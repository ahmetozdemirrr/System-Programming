#include "systemHelper.h"

volatile sig_atomic_t clientRunning = 1;

void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        clientRunning = 0;
        printf("Signal received, cancelling orders and closing...\n");
    }
}

int main(int argc, char const *argv[]) {
    printf("Process ID: %d\n", getpid());

    struct ConfigOrder config;
    checkOrderCLA(argc, argv, &config);
    setupSignalHandler(signalHandler);

    srand(time(NULL));

    int sock;
    struct sockaddr_in serverAddress;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(config.port);
    if (inet_pton(AF_INET, config.ipAddress, &serverAddress.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    logClient("Client started");

    char logMessage[LOG_MESSAGE_SIZE];
    snprintf(logMessage, sizeof(logMessage), "Connected to server at %s:%d", config.ipAddress, config.port);
    logClient(logMessage);

    // İlk başlangıç mesajını gönderme
    struct Request startRequest = {
        .pideID = 0,  // Start mesajı için özel bir işaret
        .coordinates = { .x = config.p, .y = config.q },
        .orderInfo = config
    };
    if (send(sock, &startRequest, sizeof(startRequest), 0) == -1) {
        perror("send start error");
        exit(EXIT_FAILURE);
    }

    // Müşteri siparişlerini gönderme
    for (int i = 0; i < config.numberOfClients; ++i) {
        if (!clientRunning) break;

        struct Request request;
        request.pideID = i + 1;
        request.statusCook = 0;
        request.statusDeliver = 0;
        request.clientSocket = sock;
        makeRandom(&request.coordinates, config.p, config.q);
        request.orderInfo = config;
        if (send(sock, &request, sizeof(request), 0) == -1) {
            perror("send order error");
            break;
        }
    }

    // Sunucudan yanıtları al
    char buffer[BUFFER_SIZE] = {0};
    int bytesRead;
    while (clientRunning && (bytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        //printf("%s", buffer);
        logClient(buffer);

        // Eğer "end of session" gibi bir mesaj alırsanız döngüyü sonlandırın
        if (strcmp(buffer, "All orders processed, session ending.") == 0) {
            break;
        }
    }

    if (bytesRead <= 0 && bytesRead != -1) {
        perror("recv error or server closed the connection");
    }

    close(sock);
    logClient("Client terminated normally.");
    return EXIT_SUCCESS;
}
