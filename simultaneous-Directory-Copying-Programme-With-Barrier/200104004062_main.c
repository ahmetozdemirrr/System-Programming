/* main.c */

#include "systemHelper.h"
#include "stack.h"

void handle_sigint(int signal);
void setup_sigint_handler();
void cleanup_resources(); 

int main(int argc, char const * argv[])
{
	int dummyControl; /* for checking syscall errors */

	struct timeval startTime;
	struct timeval endTime;
	
	/****************************** argument control ******************************/

	if (argc != 5)
	{
		const char * errorMessage = "Usage: ./MWCp <buffer-size> <number-of-workers> <source-dirname> <destination-dirname>\n";
		
		dummyControl = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errExitSyscall("Write error on main(checking argc param) function", dummyControl);

		exit(EXIT_FAILURE);
	}

	/******************************************************************************/
	
	setup_sigint_handler();

	bufferSize = toInteger(argv[1]);
	uint32_t workersNumber = toInteger(argv[2]);

	buffer = (RequestBody *)malloc(bufferSize * sizeof(RequestBody));

	if (buffer == NULL)
	{
		perror("Failed to allocate memory for buffer");
		exit(EXIT_FAILURE);
	}
	bufferCount = 0;

	DirPaths directories;
	strncpy(directories.sourceDirPath, argv[3], NAME_SIZE);
	strncpy(directories.destinDirPath, argv[4], NAME_SIZE);

	pthread_t workers[workersNumber];
	pthread_t manager;

/****************/
#ifdef SYS_DEBUG

		char debugMessage[CHUNK_SIZE];
	    sprintf(debugMessage, "Debugging information: bufferSize = %u, workersNumber = %u\n", bufferSize, workersNumber);

	    dummyControl = write(STDOUT_FILENO, debugMessage, strlen(debugMessage));
	    errExitSyscall("Error for writing to console on SYS_DEBUG (manager thread)", dummyControl);
#endif
/****************/
	
	pthread_mutex_init(&bufferMutex, NULL);
    pthread_cond_init(&bufferNotEmpty, NULL);
    pthread_cond_init(&bufferNotFull, NULL);
    pthread_barrier_init(&barrier, NULL, workersNumber);

	/**** TIME START ****/
	gettimeofday(&startTime, NULL);

	dummyControl = pthread_create(&manager, NULL, managerTask, (void *)&directories);
	errExitSyscall("Error on main(creating thread manager) function", dummyControl);

	for (unsigned int i = 0; i < workersNumber; ++i)
	{
		dummyControl = pthread_create(&workers[i], NULL, workerTask, NULL);
		errExitSyscall("Error on main(creating pthread workers) function", dummyControl);
	}

	dummyControl = pthread_join(manager, NULL);
	errExitSyscall("Error on joining manager thread", dummyControl);

	for (unsigned int i = 0; i < workersNumber; ++i)
	{
		dummyControl = pthread_join(workers[i], NULL);
		errExitSyscall("Error on main(joining pthread workers) function", dummyControl);
	}
	gettimeofday(&endTime, NULL);
	
	/***** TIME END *****/

    long seconds = endTime.tv_sec - startTime.tv_sec;
    long microseconds = endTime.tv_usec - startTime.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    printf("------------------ STATISTICS ------------------\n");
    printf("Total files copied: %u\n", filesCopied);
    printf("Total bytes copied: %u\n", totalBytesCopied);
    printf("Number of regular files: %u\n", numRegularFiles);
    printf("Number of FIFOs: %u\n", numFIFOFiles);
    printf("Number of directories: %u\n", numDirectories);
    printf("Number of symbolic links: %u\n", numSymbolicFiles);
    printf("Total time elapsed: %.2f seconds\n", elapsed);
    printf("------------------------------------------------\n");

	cleanup_resources();

	pthread_mutex_destroy(&bufferMutex);
    pthread_cond_destroy(&bufferNotEmpty);
    pthread_cond_destroy(&bufferNotFull);
    pthread_barrier_destroy(&barrier);

	return EXIT_SUCCESS;
}

void setup_sigint_handler() 
{
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0; // or SA_RESTART
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) 
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void handle_sigint(int signal) 
{
    pthread_mutex_lock(&bufferMutex);

    printf("Caught signal %d, terminating gracefully...\n", signal);

    killSignal = 1;    

    pthread_cond_broadcast(&bufferNotEmpty);
    pthread_mutex_unlock(&bufferMutex);

    cleanup_resources();
}

void cleanup_resources() 
{
    if (buffer != NULL)
    {
        free(buffer);
        buffer = NULL;
    }
}
