#include "helper.h"

int childCounter = 0;

int main(int argc, char const *argv[]) 
{
	char command[COMMAND_SIZE];
	char sizeBuffer[COMMAND_SIZE];

	int isReadCommand = read(STDIN_FILENO, command, COMMAND_SIZE);
	handleErrorForMinusOne("read error!", isReadCommand);

	command[strcspn(command, "\n")] = '\0';

	if (!isCommand(command)) 
    {
        const char * errMessage = "Undefined operator: ";
        char printed[MESSAGE_SIZE];

        strcpy(printed, errMessage);
        strcat(printed, argv[2]);
        strcat(printed, "\n");

        int isWriteErr = write(STDOUT_FILENO, printed, strlen(printed));
        handleErrorForMinusOne("write error!", isWriteErr);

        exit(EXIT_FAILURE);
    }

    int isReadArrSize = read(STDIN_FILENO, sizeBuffer, COMMAND_SIZE);
    handleErrorForMinusOne("read error!", isReadArrSize);

    sizeBuffer[strcspn(sizeBuffer, "\n")] = '\0';

    char * endptr;
    int arraySize = strtol(sizeBuffer, &endptr, 10);

    if (*endptr != '\0' || endptr == argv[1]) 
    {
        fprintf(stderr, "Incorrect entry: Not a valid number\n");
        exit(EXIT_FAILURE);
    }

    if (arraySize > MAX_FIFO_WRITE)
    {
        const char * sizeForFIFO = "You have exceeded the write limit to FIFO for Linux system.\n";

        int isWriteErrFIFO = write(STDOUT_FILENO, sizeForFIFO, strlen(sizeForFIFO));
        handleErrorForMinusOne("write error!", isWriteErrFIFO);

        exit(EXIT_FAILURE);
    }

    struct sigaction signalAction;
    signalAction.sa_handler = sigchld_handler;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &signalAction, NULL) == -1) 
    {
        handleError("sigaction error");
    }

    int isCreate1 = mkfifo(FIFO_PATH_1, 0666);
    handleErrorForMinusOne("mkfifo error for FIFO 1!", isCreate1);
    
    int isCreate2 = mkfifo(FIFO_PATH_2, 0666);
    handleErrorForMinusOne("mkfifo error for FIFO 2!", isCreate2);
   
    int * randomNumbers = (int *)malloc(arraySize * sizeof(int));
    if (randomNumbers == NULL) 
    {
        handleError("Memory allocation error!");
    }
    makeRandom(randomNumbers, arraySize);

    int fileDescriptor1, fileDescriptor2;

    pid_t childPID;

    for (int i = 0; i < CHILD_NUMBER; ++i) 
    {
        childPID = fork();

        if (childPID == -1) 
        {
        	free(randomNumbers);
            handleError("fork error!");
        } 

        else if (childPID == 0) 
        { 
            if (i == 0) 
            {
                int dummyFd11 = open(FIFO_PATH_1, O_RDONLY);
                handleErrorForMinusOne("Open error on FIFO 1!", dummyFd11);
                
                sleep(10);
                long long int sum = 0;

                int * readData1 = (int *)malloc(arraySize * sizeof(int));

                if (readData1 == NULL) 
			    {
			        handleError("Memory allocation error!");
			    }
                int isRead1 = read(dummyFd11, readData1, arraySize * sizeof(int));
                handleErrorForMinusOne("Read error on FIFO 1!", isRead1);

                int isCloseDummyFd11 = close(dummyFd11);
                handleErrorForMinusOne("close error", isCloseDummyFd11);

                for (int i = 0; i < arraySize; ++i) 
                {
                    sum += readData1[i];
                }

                int dummyFd12 = open(FIFO_PATH_2, O_WRONLY);
                handleErrorForMinusOne("Open error on FIFO 2!", dummyFd12);

                int isWriteSum = write(dummyFd12, &sum, sizeof(int));
                handleErrorForMinusOne("Write error on FIFO 2!", isWriteSum);

                int isCloseDummyFd12 = close(dummyFd12);
                handleErrorForMinusOne("close error", isCloseDummyFd12);

                free(readData1);
                exit(EXIT_SUCCESS);
            } 

            else if (i == 1)
            {
                int dummyFd22 = open(FIFO_PATH_2, O_RDONLY);
                handleErrorForMinusOne("Open error on FIFO 2!", dummyFd22);

                sleep(10);
                long long int summOfAll, result;

                int * readData2 = (int *)malloc(arraySize * sizeof(int));
                if (readData2 == NULL) 
			    {
			        handleError("Memory allocation error!");
			    }

                char * readCommand = (char *)malloc(COMMAND_SIZE * sizeof(char));
                if (readCommand == NULL) 
			    {
			        handleError("Memory allocation error!");
			    }

                int isRead3 = read(dummyFd22, readCommand, COMMAND_SIZE);
                handleErrorForMinusOne("Read error on FIFO 2!", isRead3);

                int isRead2 = read(dummyFd22, readData2, arraySize * sizeof(int));
                handleErrorForMinusOne("Read error on FIFO 2!", isRead2);

                int isCloseDummyFd22 = close(dummyFd22);
                handleErrorForMinusOne("close error", isCloseDummyFd22);

                int dummyFdSync = open(FIFO_PATH_2, O_RDONLY);
                handleErrorForMinusOne("Open error on FIFO 2!", dummyFdSync);

                int isRead4 = read(dummyFdSync, &summOfAll, sizeof(int));
                handleErrorForMinusOne("Read error on FIFO 2!", isRead4);

                int isCloseDummySync = close(dummyFdSync);
                handleErrorForMinusOne("close error!", isCloseDummySync);

                if (strcmp(readCommand, "multiply") == 0) 
                {
                    long long int multiplication = 1;
                   
                    for (int i = 0; i < arraySize; ++i) 
                    {
                        multiplication *= readData2[i];
                    }
                    result = multiplication;
                } 

                else if (strcmp(readCommand, "summation") == 0) 
                {
                    long long int summation = 0;

                    for (int i = 0; i < arraySize; ++i) 
                    {
                        summation += readData2[i];
                    }
                    result = summation;
                }
                char buffer[RES_BUFFER];
                
                int length = snprintf(buffer, sizeof(buffer), "Result: %lld + %lld = %lld\n", summOfAll, result, summOfAll + result);
                int isWriteResult = write(STDOUT_FILENO, buffer, length);
                handleErrorForMinusOne("write error!", isWriteResult);

                //close(fileDescriptor2);
                free(readData2);
                free(readCommand);
                exit(EXIT_SUCCESS);
            }
        }
    }    
    //close(fileDescriptor1);

    fileDescriptor2 = open(FIFO_PATH_2, O_WRONLY);
    handleErrorForMinusOne("Open FIFO 2 error!", fileDescriptor2);
   
    int isWrite3 = write(fileDescriptor2, command, COMMAND_SIZE);
    handleErrorForMinusOne("Write error on fifo 2!", isWrite3);
   
    int isWrite2 = write(fileDescriptor2, randomNumbers, arraySize * sizeof(int));
    handleErrorForMinusOne("Write error on fifo 2!", isWrite2);

    int isCloseFileDescriptor2 = close(fileDescriptor2);
    handleErrorForMinusOne("close error!", isCloseFileDescriptor2);

    fileDescriptor1 = open(FIFO_PATH_1, O_WRONLY);
    handleErrorForMinusOne("Open FIFO 1 error!", fileDescriptor1);

    int isWrite1 = write(fileDescriptor1, randomNumbers, arraySize * sizeof(int));
    handleErrorForMinusOne("Write error on fifo 1!", isWrite1);

    int isCloseFileDescriptor1 = close(fileDescriptor1);
    handleErrorForMinusOne("close error!", isCloseFileDescriptor1);

    printProceeding();

    free(randomNumbers);
    
    int isUnlinkF1 = unlink(FIFO_PATH_1);
    handleErrorForMinusOne("unlink error!", isUnlinkF1);

    int isUnlinkF2 = unlink(FIFO_PATH_2);
    handleErrorForMinusOne("unlink error!", isUnlinkF2);

    char bufferForExitStat[MESSAGE_SIZE];
    int length = snprintf(bufferForExitStat, sizeof(bufferForExitStat), "Parent process with PID %d exited with status %d\n", getpid(), EXIT_SUCCESS);
    int isWriteExitStat = write(STDOUT_FILENO, bufferForExitStat, length);
    handleErrorForMinusOne("write error!", isWriteExitStat);

    return EXIT_SUCCESS;
}
