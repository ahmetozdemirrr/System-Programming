#ifndef SAFE_FILE_H
#define SAFE_FILE_H

#include "systemHelper.h"

#define NUM_OF_DIR_FILE 256

typedef struct 
{
    char fname[256];
    sem_t read_try;   // Semaphore to attempt to read
    sem_t rmutex;     // Mutex for readers
    sem_t wmutex;     // Mutex for writers
    sem_t rsc;        // Resource access
    int reader_count;
    int writer_count;
}
safe_file;

typedef struct 
{
    safe_file files[NUM_OF_DIR_FILE];
    int capa;
    int size;
}
safe_dir;

safe_file * get_safe_file(safe_dir *, const char *);
void init_sfile(safe_file *); 
void init_safe_dir(safe_dir *, const char *);
int writer_exit_region(safe_file *);
int reader_exit_region(safe_file *);
int reader_enter_region(safe_file *);
int writer_enter_region(safe_file *);

#endif /* SAFE_FILE_H */
