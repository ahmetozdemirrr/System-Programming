#ifndef CMD_PROCESS_H
#define CMD_PROCESS_H

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>	
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define LOG_FILE "system.log"
#define INPUT_BUFFER_SIZE 3
#define TIME_BUFFER 20
#define MESSAGE_BUFFER 256
#define INPUT_BUFFER 128
#define MAX_TOKENS 10
#define MAX_WORD_SIZE 64
#define FBUFFER_SIZE 4096
#define MAX_LINES 128
#define ERR_NUM 6
#define TRUE 1

void process(int, char **, int);
char * mergeStr(const char *, const char *);
void deleteEnter(char *);
void trimQuotes(char *);
void sortMenu(int *, int *);
void logToFile(const char *);
void handleError(const char *);
void readAndTokenizeFile(const char *, char tokens[MAX_LINES][2][FBUFFER_SIZE], int *);
void prepareLogFormat(const char *, const char *, const char *, const char *);
void afterWaitErr(const char *, const int);
int searchForAdd(const char *, const char *, off_t *);
int endsWithTXT(const char *);

#endif /* CMD_PROCESS_H */
