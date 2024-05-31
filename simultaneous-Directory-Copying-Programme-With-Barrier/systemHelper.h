#pragma once

/* systemHelper.h */

#ifndef SYSTEM_HELPER_H
#define SYSTEM_HELPER_H

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>

/*#define SYS_DEBUG */
#define NUM_BASE_SYSTEM 10 /* for safe-convert string to integer (strtol) */
#define CHUNK_SIZE 1024 /* 1 KB */
#define NAME_SIZE 2048 /* for name of files */
#define TRUE 1 /* for infinite loops */

typedef struct  
{
	int FDSource;
	int FDDestin;
	char sourceDirPath[NAME_SIZE];
	char destinDirPath[NAME_SIZE];	
}
RequestBody;

typedef struct
{
	char sourceDirPath[NAME_SIZE];
	char destinDirPath[NAME_SIZE];
}
DirPaths;

extern pthread_mutex_t bufferMutex;
extern pthread_cond_t bufferNotEmpty;
extern pthread_cond_t bufferNotFull;
extern pthread_barrier_t barrier;

extern uint32_t bufferCount;
extern uint32_t bufferSize;
extern uint32_t filesCopied;
extern uint32_t totalBytesCopied;
extern uint32_t numRegularFiles;
extern uint32_t numFIFOFiles;
extern uint32_t numSymbolicFiles;
extern uint32_t numDirectories;

extern int done;
extern int killSignal;

extern RequestBody * buffer;

uint32_t toInteger(const char * source);
void errExitSyscall(const char * message, int flag);
void * managerTask(void * argument);
void * workerTask(void * argument);

#endif /* SYSTEM_HELPER_H */
