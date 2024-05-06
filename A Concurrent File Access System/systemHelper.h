#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <semaphore.h>

#define SERVER_FIFO_TEMPLATE "/tmp/server.%ld"
#define SERVER_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
#define CLIENT_FIFO_TEMPLATE "/tmp/client.%ld"
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)
#define LOG_FILE_NAME "system.log"
#define NUM_BASE_SYSTEM 10 /* for strtol function */
#define COMMUNICATION_LEN 2048
#define MAX_QUE_SIZE 128
#define MAX_CLIENTS 512
#define CWD_SIZE 256
#define BUFFER_SIZE 1024
#define REQUEST_1 "tryConnect"
#define REQUEST_2 "Connect"
#define COMMAND_1 "help"
#define COMMAND_2 "list"
#define COMMAND_3 "readF"
#define COMMAND_4 "writeT"
#define COMMAND_5 "upload"
#define COMMAND_6 "download"
#define COMMAND_7 "archServer"
#define COMMAND_8 "killServer"
#define COMMAND_9 "quit"
#define TRUE 1
#define FALSE 0
#define SEMAPHORE_NAME "/neHosSemaphore"

struct Request
{
    char input[COMMUNICATION_LEN];
    pid_t clientPID;
};

struct Response
{
    int seqNum;
    char output[COMMUNICATION_LEN];
};

struct ClientInfo
{
    pid_t pid;
    int id;
};

struct Queue
{
    int front;
    int rear;
    int capacity;
    int size;
    struct Request ** requests;
};

uint32_t toInteger(const char *); /* string to integer function */
void initLog(const char *, const char *);
void writeToLog(const char *);
void closeLog(const char *);
void errorExitForSyscall(const char *, int); /* error checker for all system call */
void checkingCommandLineServer(int, const char *); /* checker for command line arguments input (server)*/
void checkingCommandLineClient(int, const char *, const char *); /* checker for command line arguments input (client)*/
void takeClientCommands(const char *, const char *, DIR *, pid_t);
char * handleClients(struct Request *, const char *, DIR *, pid_t);
char * commandsMAN(char *);
char * errorForParam(const char *);
char * listCWD(DIR *);
char * readThisFile(const char *, int, const char *);
char * writeToFile(const char *, int , const char *, const char *);
int upload(const char *, const char *);
int download(const char *, const char *);

#endif /* SERVER_HELPER_H */
