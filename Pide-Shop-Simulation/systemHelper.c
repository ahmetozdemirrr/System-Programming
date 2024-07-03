#include "systemHelper.h"

pthread_mutex_t serverLogMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clientLogMutex = PTHREAD_MUTEX_INITIALIZER;

void logServer(const char *message)
{
    pthread_mutex_lock(&serverLogMutex);

    int fd = open(SERVER_LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("Error opening server log file");
        pthread_mutex_unlock(&serverLogMutex);
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char logEntry[LOG_MESSAGE_SIZE];
    snprintf(logEntry, sizeof(logEntry), "%04d-%02d-%02d %02d:%02d:%02d - %s\n",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             message);

    write(fd, logEntry, strlen(logEntry));
    close(fd);

    pthread_mutex_unlock(&serverLogMutex);
}

void logClient(const char *message)
{
    pthread_mutex_lock(&clientLogMutex);

    int fd = open(CLIENT_LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("Error opening client log file");
        pthread_mutex_unlock(&clientLogMutex);
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char logEntry[LOG_MESSAGE_SIZE];
    snprintf(logEntry, sizeof(logEntry), "%04d-%02d-%02d %02d:%02d:%02d - %s\n",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             message);

    write(fd, logEntry, strlen(logEntry));
    close(fd);

    pthread_mutex_unlock(&clientLogMutex);
}

void makeRandom(struct Coordinates * coordinates, int maxX, int maxY)
{
    int xRandom = rand() % (maxX + 1);
    int yRandom = rand() % (maxY + 1);

    coordinates->x = xRandom;
    coordinates->y = yRandom;
}


void errSysExit(const char *errorMessage, const int flag)
{
    if (flag == -1)
    {
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

void checkOvenCLA(int argc, char const *argv[], struct ConfigOven *config)
{
    int isWrite;

    if (argc != 6)
    {
        printf("Missing or excess arguments. Usage: ./PideShop [portnumber] [CookthreadPoolSize] [DeliveryPoolSize] [k]\n");
        exit(EXIT_FAILURE);
    }
/*
    if (strcmp(argv[0], "./PideShop") != 0)
    {
        const char *errorMessage = "Invalid command: enter only ./PideShop\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOvenCLA function", isWrite);

        exit(EXIT_FAILURE);
    }
*/
    strncpy(config->ipAddress, argv[1], 15);
    config->ipAddress[15] = '\0';

    uint32_t port = toInteger(argv[2]);
    uint32_t cookPoolSize = toInteger(argv[3]);
    uint32_t deliveryPoolSize = toInteger(argv[4]);
    uint32_t deliverySpeed = toInteger(argv[5]);

    if (cookPoolSize <= 0)
    {
        const char *errorMessage = "Cook thread pool size must be a positive integer.\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOvenCLA function", isWrite);

        exit(EXIT_FAILURE);
    }

    if (deliveryPoolSize <= 0)
    {
        const char *errorMessage = "Delivery thread pool size must be a positive integer.\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOvenCLA function", isWrite);

        exit(EXIT_FAILURE);
    }

    if (deliverySpeed <= 0)
    {
        const char *errorMessage = "Delivery speed must be a positive integer.\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOvenCLA function", isWrite);

        exit(EXIT_FAILURE);
    }
    config->cookPoolSize = cookPoolSize;
    config->deliveryPoolSize = deliveryPoolSize;
    config->deliverySpeed = deliverySpeed;
    config->port = port;
}

void checkOrderCLA(int argc, char const *argv[], struct ConfigOrder *config)
{
    int isWrite;

    if (argc != 6)
    {
        printf("Missing or excess arguments. Usage: ./HungryVeryMuch [portnumber] [numberOfClients] [p] [q]\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[0], "./HungryVeryMuch") != 0)
    {
        const char *errorMessage = "Invalid command: enter only ./HungryVeryMuch\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOrderCLA function", isWrite);

        exit(EXIT_FAILURE);
    }

    strncpy(config->ipAddress, argv[1], 15);
    config->ipAddress[15] = '\0';

    uint32_t port = toInteger(argv[2]);
    uint32_t numberOfClients = toInteger(argv[3]);
    uint32_t p = toInteger(argv[4]);
    uint32_t q = toInteger(argv[5]);

    if (numberOfClients <= 0)
    {
        const char *errorMessage = "Number of clients must be a positive integer.\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOrderCLA function", isWrite);

        exit(EXIT_FAILURE);
    }

    if (p <= 0)
    {
        const char *errorMessage = "p must be a positive integer.\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOrderCLA function", isWrite);

        exit(EXIT_FAILURE);
    }

    if (q <= 0)
    {
        const char *errorMessage = "q must be a positive integer.\n";

        isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        errSysExit("Write error on checkOrderCLA function", isWrite);

        exit(EXIT_FAILURE);
    }
    config->numberOfClients = numberOfClients;
    config->p = p;
    config->q = q;
    config->port = port;
}

uint32_t toInteger(const char *toNumber) /* I use strtol to minimize error cases */
{
    char *endptr;

    uint32_t result = strtol(toNumber, &endptr, NUM_BASE_SYSTEM); /* NUM_BASE_SYSTEM = 10 */

    if (*endptr == '\0')
    {
        return result;
    }
    const char *errorMessage = "Invalid input for number of clients!\n";

    int isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
    errSysExit("Write error on toInteger function", isWrite);

    return 0;
}

double calculateDeliveryTime(struct Coordinates destination, struct ConfigOrder orderConfig, struct ConfigOven ovenConfig)
{
    // Şehrin merkez noktası (fırının konumu)
    int centerX = orderConfig.p / 2;
    int centerY = orderConfig.q / 2;

    // Gidilecek toplam mesafe (X + Y)
    int distanceX = abs(destination.x - centerX);
    int distanceY = abs(destination.y - centerY);
    int totalDistance = distanceX + distanceY;

    // Mesafe kilometre cinsinden olduğu için kuryenin hızını da dakika/metre cinsinden kullanmalıyız
    // 1 km = 1000 metre
    double totalTime = (totalDistance * 1000) / (double)ovenConfig.deliverySpeed;

    return totalTime;
}

void setupSignalHandler(void (*handler)(int))
{
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL))
    {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }
}
