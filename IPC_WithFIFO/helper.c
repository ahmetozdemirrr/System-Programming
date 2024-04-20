#include "helper.h"

void handleErrorForMinusOne(const char * message, const int flag)
{
    if (flag == -1 && errno != EEXIST) // If FIFO is already exist; no problem, continue.
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

void handleError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int isCommand(const char * command)
{
    if (strcmp(command, "multiply") != 0 && strcmp(command, "summation") != 0)
    {
        return FALSE;
    }
    return TRUE;
}

void makeRandom(int * randomNumbers, int arraySize)
{
    srand(time(NULL));

    for (int i = 0; i < arraySize; ++i) 
    {
        randomNumbers[i] = 1 + rand() % 10;
    }
}

void sigchld_handler(int signal) 
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) 
    {
        if (WIFEXITED(status)) 
        {
            printf("Child process with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
        } 

        else 
        {
            int isUnlinkF1 = unlink(FIFO_PATH_1);
            handleErrorForMinusOne("unlink error!", isUnlinkF1);

            int isUnlinkF2 = unlink(FIFO_PATH_2);
            handleErrorForMinusOne("unlink error!", isUnlinkF2);
            
            kill(getppid(), SIGTERM);
            printf("Child process with PID %d terminated abnormally\n", pid);
        }
        childCounter++;
    }
}

void printProceeding()
{
    const char * proceedingMess = "proceeding\n";

    while (childCounter < CHILD_NUMBER) 
    {
        int isWriteProceeding = write(STDOUT_FILENO, proceedingMess, strlen(proceedingMess));
        handleErrorForMinusOne("write error!", isWriteProceeding);
        sleep(2);
    }
}
