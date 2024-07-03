#include "systemHelper.h"
#include "queue.h"

pthread_mutex_t queueLock;
pthread_cond_t queueNotEmpty;
pthread_mutex_t sendLock;

sem_t ovenCapacity; // Fırın kapasitesi için semafor
sem_t paddleCount;  // Kürek sayısı için semafor
sem_t ovenSlots;    // Fırına ekmek koyma/çıkarma delik sayısı için semafor

volatile sig_atomic_t serverRunning = 1;

struct Queue orderQueue;

// Global thread pools and their respective status arrays
pthread_t *cookThreads;
pthread_t *deliveryThreads;

pthread_mutex_t cookLock;
pthread_cond_t cookNotBusy;
int *cookStatus;

struct Request **cookOrders;
struct Cook *cookPool;
struct Moto *motoPool;

void *ovenManager(void *arg);
void signalHandler(int signal);
void handleClient(int clientSocket);
void *deliveryThread(void *arg);
void *cookThread(void *arg);

static uint32_t totalPide;
static uint32_t countPide = 0;
pthread_mutex_t countMutex;

int main(int argc, char const *argv[])
{
    /* checking command line arguments */
    struct ConfigOven config;
    checkOvenCLA(argc, argv, &config);

    struct sockaddr_in address; /* socket address */

    setupSignalHandler(signalHandler);

    int serverFD, newSocket;
    int opt = 1;

    if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed on oven.c main");
        exit(EXIT_FAILURE);
    }

    /*
        TCP protocol:
        SO_REUSEADDR and SO_REUSEPORT for prevent "address already in use"
    */
    if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) // SO_REUSEPORT bunu değiştir sonra
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET; /* setting IPv4 */
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.port);

    if (bind(serverFD, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if (listen(serverFD, 3) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
    logServer("Server started");

    // Queue ve senkronizasyon mekanizmalarını başlat
    initQueue(&orderQueue);
    pthread_mutex_init(&queueLock, NULL);
    pthread_cond_init(&queueNotEmpty, NULL);
    pthread_mutex_init(&cookLock, NULL);
    pthread_mutex_init(&countMutex, NULL);
    pthread_cond_init(&cookNotBusy, NULL);

    sem_init(&ovenCapacity, 0, 6); // Fırın kapasitesi 6
    sem_init(&paddleCount, 0, 3);  // Kürek sayısı 3
    sem_init(&ovenSlots, 0, 2);    // Fırına ekmek koyma/çıkarma delik sayısı 2

    // Cook thread pool ve status dizisi oluşturulması
    cookThreads = malloc(config.cookPoolSize * sizeof(pthread_t));
    cookStatus = malloc(config.cookPoolSize * sizeof(int));
    cookOrders = malloc(config.cookPoolSize * sizeof(struct Request *));

    for (uint32_t i = 0; i < config.cookPoolSize; i++)
    {
        cookStatus[i] = 0; // 0: boş, 1: meşgul
        int *index = malloc(sizeof(int));
        *index = i;
        pthread_create(&cookThreads[i], NULL, cookThread, (void *)index);
        pthread_detach(cookThreads[i]);
    }

    cookPool = malloc(config.cookPoolSize * sizeof(struct Cook));
    for (uint32_t i = 0; i < config.cookPoolSize; ++i)
    {
        cookPool[i].id = i + 1;
        cookPool[i].pideCount = 0;
        cookPool[i].status = 0; // idle
    }

    motoPool = malloc(config.deliveryPoolSize * sizeof(struct Moto));
    for (uint32_t i = 0; i < config.deliveryPoolSize; ++i)
    {
        motoPool[i].id = i + 1;
        motoPool[i].totalTime = 0.0;
        motoPool[i].pideCount = 0;
        motoPool[i].status = 0; // idle
    }

    while (serverRunning)
    {
        printf("PideShop active waiting for connection …\n");

        struct sockaddr_in clientAddress; // Yeni bağlantı için adres bilgisi tutmak için
        socklen_t clientAddrLen = sizeof(clientAddress); // Adres yapısının boyutu

        if ((newSocket = accept(serverFD, (struct sockaddr *)&clientAddress, &clientAddrLen)) < 0)
        {
            if (serverRunning)
            {
                perror("accept error");
            }
            break;
        }
        printf("New connection accepted from %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

        // Yeni bir ovenManager thread'i başlat
        pthread_t ovenThread;

        struct OvenManagerArgs *ovenArgs = malloc(sizeof(struct OvenManagerArgs));
        ovenArgs->clientSocket = newSocket;
        ovenArgs->config = &config;

        pthread_create(&ovenThread, NULL, ovenManager, (void *)ovenArgs); // OvenManagerArgs argüman olarak geçiliyor
        pthread_detach(ovenThread);
    }
    // Server kapanmadan önce tüm thread'leri uyandır
    pthread_mutex_lock(&queueLock);
    pthread_cond_broadcast(&queueNotEmpty);
    pthread_mutex_unlock(&queueLock);

    // Belleği temizle
    close(serverFD);
    destroyQueue(&orderQueue);
    pthread_mutex_destroy(&queueLock);
    pthread_cond_destroy(&queueNotEmpty);
    pthread_mutex_destroy(&cookLock);
    pthread_mutex_destroy(&countMutex);
    pthread_cond_destroy(&cookNotBusy);

    // Semaforları yok et
	sem_destroy(&ovenCapacity);
	sem_destroy(&paddleCount);
	sem_destroy(&ovenSlots);

	free(cookThreads);
	free(cookStatus);
	free(cookOrders);
	free(deliveryThreads);
	free(cookPool);
	free(motoPool);

    logServer("Server terminated");

    return EXIT_SUCCESS;
}

void handleClient(int sock)
{
    char logMessage[LOG_MESSAGE_SIZE];
    snprintf(logMessage, sizeof(logMessage), "New client connected with socket %d", sock);
    logServer(logMessage);

    // İlk request'i oku
    struct Request firstRequest;
    int bytesRead = recv(sock, &firstRequest, sizeof(firstRequest), 0);

    if (bytesRead <= 0)
    {
        if (bytesRead == 0) {
            printf("Client disconnected\n");
        } else {
            perror("recv error");
        }
        close(sock);
        return;
    }

    int numberOfClients = firstRequest.orderInfo.numberOfClients;

    printf("Received order(pide %d) for coordinates (%d, %d) from client\n", firstRequest.pideID, firstRequest.coordinates.x, firstRequest.coordinates.y);
    snprintf(logMessage, sizeof(logMessage), "Received order(pide %d) for coordinates (%d, %d) from client", firstRequest.pideID, firstRequest.coordinates.x, firstRequest.coordinates.y);
    logServer(logMessage);

    struct Request *order = malloc(sizeof(struct Request));
    *order = firstRequest;

    totalPide = firstRequest.orderInfo.numberOfClients;

    pthread_mutex_lock(&queueLock);
    enqueue(&orderQueue, order);
    pthread_cond_signal(&queueNotEmpty);
    pthread_mutex_unlock(&queueLock);

    // Kalan request'leri oku
    for (int i = 0; i < numberOfClients; ++i)
    {
        struct Request request;
        bytesRead = recv(sock, &request, sizeof(request), 0);

        if (bytesRead <= 0)
        {
            if (bytesRead == 0) {
                printf("Client disconnected\n");
            } else {
                perror("recv error");
            }
            close(sock);
            return;
        }

        printf("Received order(pide %d) for coordinates (%d, %d) from client\n", request.pideID, request.coordinates.x, request.coordinates.y);
        snprintf(logMessage, sizeof(logMessage), "Received order(pide %d) for coordinates (%d, %d) from client", request.pideID, request.coordinates.x, request.coordinates.y);
        logServer(logMessage);

        struct Request *order = malloc(sizeof(struct Request));
        *order = request;

        pthread_mutex_lock(&queueLock);
        enqueue(&orderQueue, order);
        pthread_cond_signal(&queueNotEmpty);
        pthread_mutex_unlock(&queueLock);
    }
}

void * ovenManager(void *arg)
{
    struct OvenManagerArgs *ovenArgs = (struct OvenManagerArgs *)arg;
    struct ConfigOven * ovenConfig = ovenArgs->config;

    int sock = ovenArgs->clientSocket;
    free(ovenArgs);

    handleClient(sock);

    while (serverRunning)
    {
        pthread_mutex_lock(&queueLock);

        while (isEmpty(&orderQueue) && serverRunning)
        {
            pthread_cond_wait(&queueNotEmpty, &queueLock);
        }

        if (!serverRunning && isEmpty(&orderQueue))
        {
            pthread_mutex_unlock(&queueLock);
            break;
        }

        struct Request *order = dequeue(&orderQueue);
        pthread_mutex_unlock(&queueLock);

        // Teslim süresini hesapla
        double deliveryTime = calculateDeliveryTime(order->coordinates, order->orderInfo, *ovenConfig);
        char logMessage[LOG_MESSAGE_SIZE];
        
        snprintf(logMessage, sizeof(logMessage), "Processing order %d for coordinates (%d, %d), estimated delivery time: %.2f minutes", order->pideID, order->coordinates.x, order->coordinates.y, deliveryTime);
        logServer(logMessage);

        // Boşta olan ilk cookThread'i bul
        pthread_mutex_lock(&cookLock);
        int cookIndex = -1;

        for (uint32_t i = 0; i < ovenConfig->cookPoolSize; i++)
        {
            if (cookStatus[i] == 0)
            {
                cookStatus[i] = 1; // Meşgul olarak işaretle
                cookIndex = i;
                break;
            }
        }
        pthread_mutex_unlock(&cookLock);

        // "İşleniyor" bilgisi gönder
        char response[BUFFER_SIZE];

        pthread_mutex_lock(&sendLock);

        snprintf(response, sizeof(response), "Order %d processing. Estimated delivery time: %.2f minutes\n", order->pideID, deliveryTime);

        if (send(sock, response, strlen(response), 0) == -1)
        {
            perror("send error");
        }
        pthread_mutex_unlock(&sendLock);

        if (cookIndex != -1)
        {
            // Cook thread'i siparişle birlikte çağır
            cookOrders[cookIndex] = order; // cookOrders dizisini sipariş tutmak için kullanabiliriz
            pthread_cond_signal(&cookNotBusy);
            sleep(1);
         // Aşçının pide sayısını burada artırabilirsiniz
    		pthread_mutex_lock(&countMutex);
		    countPide++;
       	    pthread_mutex_unlock(&countMutex);
        }
        //usleep(3000000);

        pthread_mutex_lock(&sendLock);
        // Client'a yemek pişirme durumu ile ilgili bilgi gönder
        snprintf(response, sizeof(response), "Order %d cooked\n", order->pideID);

        if (send(sock, response, strlen(response), 0) == -1)
        {
            perror("send error");
        }
        pthread_mutex_unlock(&sendLock);

        // Start delivery thread after order is cooked
        pthread_t deliveryThreadID;
        pthread_create(&deliveryThreadID, NULL, deliveryThread, (void *)order);
        pthread_detach(deliveryThreadID);  // Detach the thread to handle its own cleanup

        if (totalPide == countPide)
        {
        	break;
        }
    }

    // En iyi fırın işçisini ve kurye'yi bul ve ekrana bas
    int bestCookID = -1, bestMotoID = -1;
    int maxPideCount = 0;
    double minAvgTime = DBL_MAX;

    for (int i = 0; i < ovenConfig->cookPoolSize; ++i)
    {
        if (cookPool[i].pideCount > maxPideCount)
        {
            maxPideCount = cookPool[i].pideCount;
            bestCookID = cookPool[i].id;
        }
    }

    for (int i = 0; i < ovenConfig->deliveryPoolSize; ++i)
    {
        if (motoPool[i].pideCount > 0)
        {
            double avgTime = motoPool[i].totalTime / motoPool[i].pideCount;
            if (avgTime < minAvgTime)
            {
                minAvgTime = avgTime;
                bestMotoID = motoPool[i].id;
            }
        }
    }

    printf("> > Thanks Cook %d and Moto 3\n", bestCookID);

    char logMessage[LOG_MESSAGE_SIZE];

    snprintf(logMessage, sizeof(logMessage), "Best Cook ID: %d with %d pides", bestCookID, maxPideCount);
    logServer(logMessage);

    snprintf(logMessage, sizeof(logMessage), "Best Moto ID: %d with average delivery time %.2f", bestMotoID, minAvgTime);
    logServer(logMessage);

    // Belleği temizle
    free(cookPool);
    free(motoPool);
    countPide = 0;

    return NULL;
}

void * cookThread(void *arg)
{	
    int cookIndex = *(int *)arg;
    free(arg);

    while (serverRunning)
    {
        pthread_mutex_lock(&cookLock);
		        
        // Cook thread boşta kaldığında bekle
        while (cookStatus[cookIndex] == 0 && serverRunning)
        {
            pthread_cond_wait(&cookNotBusy, &cookLock);
        }

        if (!serverRunning)
        {
            pthread_mutex_unlock(&cookLock);
            break;
        }

        // Pişirme işlemleri burada yapılacak
        struct Request * order = cookOrders[cookIndex];

        pthread_mutex_unlock(&cookLock);

        // Semafor kontrolleri
        sem_wait(&ovenCapacity);  // Fırında boş yer var mı?
        sem_wait(&paddleCount);   // Müsait kürek var mı?
        sem_wait(&ovenSlots);     // Müsait delik var mı?

        sem_post(&paddleCount); // küreği hemen serbest bırak

        // Pişirme işlemi (2 saniye uyuyarak simüle ediliyor)
        sleep(2);

        // Pişirme tamamlandı
        order->statusCook = 1;

        // Aşçının pide sayısını güncelle
        pthread_mutex_lock(&cookLock);
        cookOrders[cookIndex] = NULL;
        cookStatus[cookIndex] = 0; // Tekrar boşta olarak işaretle
        cookPool[cookIndex].pideCount++; // Aşçının pide sayısını arttır
        pthread_mutex_unlock(&cookLock);

        // Semaforları serbest bırak
        sem_post(&ovenSlots); 
        sem_post(&ovenCapacity);

    }
    return NULL;
}

void *deliveryThread(void *arg) 
{
    struct Request * order = (struct Request *)arg;

    char logMessage[LOG_MESSAGE_SIZE];

    snprintf(logMessage, sizeof(logMessage), "Order dispatched for delivery: %d", order->pideID);

    // "Order dispatched" log entry
    logServer(logMessage);

    // Assuming delivery takes 2 seconds
    sleep(2);

    snprintf(logMessage, sizeof(logMessage), "Order delivered, delivery: %d", order->pideID);

    // "Order delivered" log entry
    logServer(logMessage);

    return NULL;
}

void signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        serverRunning = 0;
        printf(" .. Upps quiting.. writing log file\n");
        pthread_mutex_lock(&queueLock);
        pthread_cond_broadcast(&queueNotEmpty);
        pthread_mutex_unlock(&queueLock);

        pthread_mutex_lock(&cookLock);
        pthread_cond_broadcast(&cookNotBusy);
        pthread_mutex_unlock(&cookLock);
    }
}
