#include "safeFile.h"

void init_safe_dir(safe_dir * sdir, const char * directoryPath) 
{
    DIR *dir = opendir(directoryPath);

    if (!dir) 
    {
        perror("Failed to open directory");
        return;
    }

    struct dirent *entry;
    int idx = 0;

    while ((entry = readdir(dir)) != NULL && idx < 100) 
    {
        if (entry->d_type == DT_REG) 
        {
            strcpy(sdir->files[idx].fname, entry->d_name);
            init_sfile(&sdir->files[idx]);
            idx++;
        }
    }
    sdir->size = idx;
    closedir(dir);
}

void init_sfile(safe_file * sfile) 
{
    sem_init(&sfile->read_try, 0, 1);
    sem_init(&sfile->rmutex, 0, 1);
    sem_init(&sfile->wmutex, 0, 1);
    sem_init(&sfile->rsc, 0, 1);

    sfile->reader_count = 0;
    sfile->writer_count = 0;
}

safe_file * get_safe_file(safe_dir * sdir, const char * filename) 
{
    for (int i = 0; i < sdir->size; i++) 
    {
        if (strcmp(sdir->files[i].fname, filename) == 0) 
        {
            return &sdir->files[i];
        }
    }
    return NULL;
}

int reader_enter_region(safe_file * sfile) 
{
    if (sem_wait(&sfile->read_try) == -1) 
    {
        perror("sem_wait on read_try");
        return -1;
    }

    if (sem_wait(&sfile->rmutex) == -1) 
    {
        perror("sem_wait on rmutex");
        return -1;
    }

    sfile->reader_count++;

    if (sfile->reader_count == 1) 
    {
        if (sem_wait(&sfile->rsc) == -1) 
        {
            perror("sem_wait on rsc");
            return -1;
        }
    }

    if (sem_post(&sfile->rmutex) == -1) 
    {
        perror("sem_post on rmutex");
        return -1;
    }

    if (sem_post(&sfile->read_try) == -1) 
    {
        perror("sem_post on read_try");
        return -1;
    }
    return 0;
}

int reader_exit_region(safe_file * sfile) 
{
    if (sem_wait(&sfile->rmutex) == -1) 
    {
        perror("sem_wait on rmutex");
        return -1;
    }

    sfile->reader_count--;

    if (sfile->reader_count == 0) 
    {
        if (sem_post(&sfile->rsc) == -1) 
        {
            perror("sem_post on rsc");
            return -1;
        }
    }

    if (sem_post(&sfile->rmutex) == -1) 
    {
        perror("sem_post on rmutex");
        return -1;
    }
    return 0;
}

int writer_enter_region(safe_file * sfile) 
{
    if (sem_wait(&sfile->wmutex) == -1) 
    {
        perror("sem_wait on wmutex");
        return -1;
    }

    sfile->writer_count++;

    if (sfile->writer_count == 1) 
    {
        if (sem_wait(&sfile->read_try) == -1) 
        {
            perror("sem_wait on read_try");
            return -1;
        }
    }

    if (sem_post(&sfile->wmutex) == -1) 
    {
        perror("sem_post on wmutex");
        return -1;
    }

    if (sem_wait(&sfile->rsc) == -1) 
    {
        perror("sem_wait on rsc");
        return -1;
    }
    return 0;
}

int writer_exit_region(safe_file * sfile) 
{
    if (sem_post(&sfile->rsc) == -1) 
    {
        perror("sem_post on rsc");
        return -1;
    }

    if (sem_wait(&sfile->wmutex) == -1) 
    {
        perror("sem_wait on wmutex");
        return -1;
    }

    sfile->writer_count--;

    if (sfile->writer_count == 0) 
    {
        if (sem_post(&sfile->read_try) == -1) 
        {
            perror("sem_post on read_try");
            return -1;
        }
    }

    if (sem_post(&sfile->wmutex) == -1) 
    {
        perror("sem_post on wmutex");
        return -1;
    }
    return 0;
}
