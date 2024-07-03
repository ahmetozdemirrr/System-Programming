#pragma once

/* systemHelper.h */

#ifndef SYSTEM_HELPER_H
#define SYSTEM_HELPER_H

#include <poll.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/un.h>
#include <termios.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_LOG_FILE "server.log"
#define CLIENT_LOG_FILE "client.log"
#define SOCKET_PATH "/tmp/pide_socket"
#define NUM_BASE_SYSTEM 10
#define TRUE 1
#define BUFFER_SIZE 2048
#define LOG_MESSAGE_SIZE 4096

struct Coordinates /* for order coordinates */
{
	int x;
	int y;
};

struct ConfigOven /* command line arguments */
{
	char ipAddress[16];
    uint32_t port;
    uint32_t cookPoolSize;
    uint32_t deliveryPoolSize;
    uint32_t deliverySpeed;
};

struct ConfigOrder /* command line arguments */
{
	char ipAddress[16];
    uint32_t port;
    uint32_t numberOfClients;
    uint32_t p;
    uint32_t q;
};

struct Request 
{
    int pideID;
    int statusCook; // pişti pişmedi
    int statusDeliver;
    int clientSocket;
    struct Coordinates coordinates; // pidenin koordinatı
    struct ConfigOrder orderInfo; // orderın p ve q bilgisi için
};

struct Cook 
{
    int id;
    int pideCount;
    int status; // 0: idle, 1: busy
};

struct Moto 
{
    int id;
    double totalTime;
    int pideCount;
    int status; // 0: idle, 1: busy
};

struct OvenManagerArgs 
{
    int clientSocket;
    struct ConfigOven * config;
};

void makeRandom(struct Coordinates * coordinates, int maxX, int maxY);
void errSysExit(const char * message, const int flag);
void checkOvenCLA(int argc, char const * argv[], struct ConfigOven * config);
void checkOrderCLA(int argc, char const * argv[], struct ConfigOrder * config);
void setupSignalHandler(void (* handler)(int));
double calculateDeliveryTime(struct Coordinates destination, struct ConfigOrder orderConfig, struct ConfigOven ovenConfig);
void logServer(const char *message);
void logClient(const char *message);

uint32_t toInteger(const char * toNumber);

#endif /* SYSTEM_HELPER_H */
