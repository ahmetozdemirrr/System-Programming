#ifndef HELPER_H
#define HELPER_H

#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define CHILD_NUMBER 2
#define RES_BUFFER 64
#define BUFFER_SIZE 512
#define COMMAND_SIZE 32
#define MESSAGE_SIZE 100
#define MAX_FIFO_WRITE 16000
#define FIFO_PATH_1 "fifo1" // Path to the named pipe 1
#define FIFO_PATH_2 "fifo2" // Path to the named pipe 2
#define TRUE 1
#define FALSE 0

extern int childCounter;

void handleErrorForMinusOne(const char * message, const int flag);
void handleError(const char * message);
void makeRandom(int * randomNumbers, int arraySize);
void sigchld_handler(int sig);
void printProceeding();
int isCommand(const char * command);

#endif /* HELPER_H */
