#include "systemHelper.h"
#include "stack.h"

pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bufferNotEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t bufferNotFull = PTHREAD_COND_INITIALIZER;

uint32_t bufferSize;
uint32_t bufferCount;
uint32_t filesCopied = 0;
uint32_t totalBytesCopied = 0;
uint32_t numFIFOFiles = 0;
uint32_t numRegularFiles = 0;
uint32_t numSymbolicFiles = 0;
uint32_t numDirectories = 0;

int done = 0;
int killSignal = 0;

RequestBody * buffer;

uint32_t toInteger(const char *toNumber)
{
    char * endptr;

    uint32_t result = strtol(toNumber, &endptr, NUM_BASE_SYSTEM);

    if (*endptr == '\0')
    {
        return result;
    }
    const char *errorMessage = "Invalid input for unsigned integer value(workers number or buffer size)\n";
   
    int isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
    errExitSyscall("Write error on toInteger function", isWrite);
    
    return 0;
}

void errExitSyscall(const char *errorMessage, int flag)
{
    if (flag == -1)
    {
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

void * managerTask(void * argument)
{
    DirPaths * initialDirs = (DirPaths *)argument;
    StackNode * stack = createStackNode(*initialDirs);

    while (!isStackEmpty(stack) && !killSignal)
    {
        DirPaths currentDirs = pop(&stack);

        const char * sourcePath = currentDirs.sourceDirPath;
        const char * destinPath = currentDirs.destinDirPath;

        DIR * sourceDir = opendir(sourcePath);

        if (!sourceDir)
        {
            perror("Failed to open source directory on manager thread");
            continue;
        }

        struct dirent * dEntry;

        while ((dEntry = readdir(sourceDir)) != NULL)
        {
            if (strcmp(dEntry->d_name, ".") == 0 || strcmp(dEntry->d_name, "..") == 0)
            {
                continue;
            }
            char sourceFileName[NAME_SIZE];
            char destinFileName[NAME_SIZE];

            snprintf(sourceFileName, NAME_SIZE, "%s/%s", sourcePath, dEntry->d_name);
            snprintf(destinFileName, NAME_SIZE, "%s/%s", destinPath, dEntry->d_name);

            if (strncmp(destinPath, sourceFileName, strlen(destinPath)) == 0)
            {
                continue;
            }

            if (dEntry->d_type == DT_REG || dEntry->d_type == DT_FIFO || dEntry->d_type == DT_LNK)
            {
                int sourceFD = open(sourceFileName, O_RDONLY);
                int destinFD = -1;
               
                if (sourceFD != -1)
                {
                    destinFD = open(destinFileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }

                pthread_mutex_lock(&bufferMutex);

                while (bufferCount == bufferSize && !killSignal)
                {
                    pthread_cond_wait(&bufferNotFull, &bufferMutex);
                }

                if (killSignal)
                {
                    if (sourceFD != -1) close(sourceFD);
                    if (destinFD != -1) close(destinFD);
                    
                    pthread_mutex_unlock(&bufferMutex);
                    break;
                }

                buffer[bufferCount].FDSource = sourceFD;
                buffer[bufferCount].FDDestin = destinFD;
                strcpy(buffer[bufferCount].sourceDirPath, sourceFileName);
                strcpy(buffer[bufferCount].destinDirPath, destinFileName);
                bufferCount++;

                if (dEntry->d_type == DT_REG)
                {
                    numRegularFiles++;
                }

                else if (dEntry->d_type == DT_FIFO)
                {
                    numFIFOFiles++;
                }

                else if (dEntry->d_type == DT_LNK)
                {
                    numSymbolicFiles++;
                }

                pthread_cond_signal(&bufferNotEmpty);
                pthread_mutex_unlock(&bufferMutex);

#ifdef SYS_DEBUG

                char debugMessage[CHUNK_SIZE];
                int debugMessageLen = snprintf(debugMessage, sizeof(debugMessage), "Manager added file: %s to buffer\n", sourceFileName);
                
                if (debugMessageLen < 0 || debugMessageLen >= sizeof(debugMessage))
                {
                    fprintf(stderr, "Debug message was truncated\n");
                }
                
                else
                {
                    int dummyControl = write(STDOUT_FILENO, debugMessage, strlen(debugMessage));
                    errExitSyscall("Error for writing to console on SYS_DEBUG (manager thread)", dummyControl);
                }
#endif

            }

            else if (dEntry->d_type == DT_DIR)
            {
                if (strncmp(destinPath, sourceFileName, strlen(destinPath)) == 0)
                {
                    continue;
                }

                if (mkdir(destinFileName, 0777) == -1 && errno != EEXIST)
                {
                    perror("Failed to create destination directory");
                    continue;
                }
                numDirectories++;

                DirPaths subDirectories;

                strcpy(subDirectories.sourceDirPath, sourceFileName);
                strcpy(subDirectories.destinDirPath, destinFileName);

                push(&stack, subDirectories);
            }
        }
        closedir(sourceDir);
    }
    pthread_mutex_lock(&bufferMutex);
    done = 1;
    pthread_cond_broadcast(&bufferNotEmpty);
    pthread_mutex_unlock(&bufferMutex);

    clearStack(&stack);
    pthread_exit(0);
}

void * workerTask(void * argument)
{
    while (TRUE)
    {
        pthread_mutex_lock(&bufferMutex);

        while (bufferCount == 0 && !done && !killSignal)
        {
            pthread_cond_wait(&bufferNotEmpty, &bufferMutex);
        }

        if ((bufferCount == 0 && done) || killSignal)
        {
        pthread_cond_signal(&bufferNotFull);
            
            pthread_mutex_unlock(&bufferMutex);

            break;
        }

        RequestBody request = buffer[--bufferCount];

        pthread_cond_signal(&bufferNotFull);
        pthread_mutex_unlock(&bufferMutex);

        if (request.FDSource == -1 || request.FDDestin == -1)
        {
            if (request.FDSource != -1) close(request.FDSource);
            if (request.FDDestin != -1) close(request.FDDestin);

            fprintf(stderr, "Error: Failed to open file %s or %s\n", request.sourceDirPath, request.destinDirPath);
            continue;
        }

        char bufferData[CHUNK_SIZE];
        ssize_t bytesRead;

        while ((bytesRead = read(request.FDSource, bufferData, CHUNK_SIZE)) > 0)
        {
            ssize_t bytesWritten = write(request.FDDestin, bufferData, bytesRead);

            if (bytesWritten != bytesRead)
            {
                perror("Error writing to destination file");
                break;
            }

            pthread_mutex_lock(&bufferMutex);
            totalBytesCopied += bytesWritten;
            pthread_mutex_unlock(&bufferMutex);
        }
        close(request.FDSource);
        close(request.FDDestin);

        pthread_mutex_lock(&bufferMutex);
        filesCopied++;
        pthread_mutex_unlock(&bufferMutex);

#ifdef SYS_DEBUG

        char debugMessage[CHUNK_SIZE];
        int debugMessageLen = snprintf(debugMessage, sizeof(debugMessage), "Worker copied file from %s to %s\n", request.sourceDirPath, request.destinDirPath);
        
        if (debugMessageLen < 0 || debugMessageLen >= sizeof(debugMessage))
        {
            fprintf(stderr, "Debug message was truncated\n");
        }

        else
        {
            int dummyControl = write(STDOUT_FILENO, debugMessage, strlen(debugMessage));
            errExitSyscall("Error for writing to console on SYS_DEBUG (worker thread)", dummyControl);
        }
#endif

    }
    pthread_exit(0);
}
