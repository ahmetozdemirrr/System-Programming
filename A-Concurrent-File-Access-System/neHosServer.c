/* neHosServer.c */

#include "systemHelper.h"
#include "queue.h"

/*
	argv[0] -> neHosServer
	argv[1] -> <dirname>
	argv[2] -> <max. #ofClients>
*/

static volatile sig_atomic_t quit = 0;
static char clientFIFO[CLIENT_FIFO_NAME_LEN];
static char serverFIFO[SERVER_FIFO_NAME_LEN];
static int clientCounter = 1;
static struct ClientInfo clientInfos[MAX_CLIENTS];
static struct ClientInfo clientInfosToKill[MAX_CLIENTS];
static int dummyFD, serverFD;

void sigint_handler(int);
void setup_signal_handler();
void close_server();
void terminate_all_childs();

extern sem_t * logSem;

int main(int argc, char const * argv[])
{
	if (argc < 3) 
	{
        fprintf(stderr, "Usage: %s <dirname> <max. #ofClients>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

	checkingCommandLineServer(argc, argv[0]);
	uint32_t numOfClient = toInteger(argv[2]);
	
	setup_signal_handler();

	int isOK;
	
	struct Queue waitQueue;
	if (initQueue(&waitQueue) == -1)
	{
		perror("Queue initialization error!");
		exit(EXIT_FAILURE);
	}

	struct Request req;
	struct Response resp;

	const char * directoryPath = argv[1];
	DIR * serverDirectory;

	if (((isOK = mkdir(directoryPath, S_IRWXU | S_IWUSR | S_IRUSR | S_IXUSR | S_IWGRP | S_IRGRP)) == -1) && (errno != EEXIST))
	{
		perror("mkdir error for serverDirectory");
		exit(EXIT_FAILURE);
	}

	if ((serverDirectory = opendir(directoryPath)) == NULL)
	{
		perror("opendir error for serverDirectory");
		exit(EXIT_FAILURE);
	}

	snprintf(serverFIFO, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE, (long) getpid());
	unlink(serverFIFO); /* Ensure the FIFO does not already exist */

	if (mkfifo(serverFIFO, 0666) == -1) 
	{
        perror("Failed to create serverFIFO");
        exit(EXIT_FAILURE);
    }

    initLog("myLogSem", LOG_FILE_NAME);

    printf(">> Server Started PID %d...\n", getpid());
	printf(">> Waiting for clients...\n");

	serverFD = open(serverFIFO, O_RDONLY | O_NONBLOCK);
    if (serverFD == -1) 
    {
        perror("Failed to open serverFIFO");
        exit(EXIT_FAILURE);
    }

	dummyFD = open(serverFIFO, O_WRONLY); /* Open an extra write descriptor, so that we never see EOF */
	if (dummyFD == -1)
	{
		close(dummyFD);
		unlink(serverFIFO);
		perror("open error for dummyFD");
		exit(EXIT_FAILURE);
	}

	int clientProcesses = 0;

	while (!quit)
	{
		int childStatus, toConnect = 0;
		pid_t finishedPID = waitpid(-1, &childStatus, WNOHANG);
		pid_t clientPID;

		if (finishedPID > 0)
		{
			for (int i = 0; i < clientCounter; i++) 
			{
                if (clientInfos[i].pid == finishedPID) 
                {
                    clientProcesses--;

                    if (clientInfos[i].id < 10)
                    {
                    	printf(">> Client0%d disconnected..\n", clientInfos[i].id);
                    }

                    else
                    {
                    	printf(">> Client%d disconnected..\n", clientInfos[i].id);
                    }
                    break;
                }
            }
		}

    	if (read(serverFD, &req, sizeof(struct Request)) == sizeof(struct Request)) 
    	{
	        clientPID = req.clientPID;
	        toConnect = 1;

	        if (clientProcesses >= numOfClient || !isEmpty(&waitQueue)) 
	        {
	        	if ((strcmp(req.input, REQUEST_1) == 0)) /* Handling tryConnect request */
			    {
		        	snprintf(clientFIFO, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) clientPID);
		            
		            resp.seqNum = -1; /* Indicates error or wait situation */
		            strcpy(resp.output, "There is currently no free space on the server!");
		            
		            int fifoWriteFD = open(clientFIFO, O_WRONLY);

		            isOK = write(fifoWriteFD, &resp, sizeof(struct Response));
		            errorExitForSyscall("Write error on fifoWriteFD", isOK);

		            close(fifoWriteFD);
		            continue; /* Skip the rest of the loop */
			    }	
		        printf("Connection request PID %d... Que FULL\n", clientPID);

	            enqueue(&waitQueue, &req);
	            toConnect = 0;
	        }
	    } 

	    else if (errno != EAGAIN && errno != EINTR) 
	    {
	        perror("Read error on serverFIFO");
	        break;
	    }

	    if (!toConnect && !isEmpty(&waitQueue) && (clientProcesses < numOfClient)) 
	    {
	        struct Request* currentClient = dequeue(&waitQueue);
	        req = *currentClient;
	        clientPID = req.clientPID;
	        toConnect = 1;
	    }

		if (toConnect == 1)
		{
	    	snprintf(clientFIFO, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) clientPID);

			int fifoWriteFD;

			while((fifoWriteFD = open(clientFIFO, O_WRONLY)) == -1 && errno == EINTR); 
			errorExitForSyscall("open error for fifoWriteFD", fifoWriteFD);

			memset(&resp, 0, sizeof(struct Response));
			resp.seqNum = clientProcesses + waitQueue.size;
			strcpy(resp.output, "null");
			
			if (write(fifoWriteFD, &resp, sizeof(struct Response)) == -1)
            {
                perror("write error on fifoWriteFD");
                exit(EXIT_FAILURE);
            }
			isOK = close(fifoWriteFD);
			errorExitForSyscall("close error fifoWriteFD", isOK);
		    
		    pid_t currClientPID = fork();
			errorExitForSyscall("Fork error for client forking", currClientPID); /* if(pid == -1) */

			if (currClientPID == 0) 	
			{
				close(serverFD);
				close(dummyFD);

                takeClientCommands(clientFIFO, directoryPath, serverDirectory, clientPID);

                exit(EXIT_SUCCESS);				
			}

			else
			{
				clientProcesses++;
                clientInfos[clientCounter].pid = currClientPID;
                clientInfosToKill[clientCounter].pid = clientPID;
                clientInfos[clientCounter].id = clientCounter;

                char bufferLog[128];

                if (clientCounter < 10)
                {
                	snprintf(bufferLog, sizeof(bufferLog), "Client PID %d connected as \"client0%d\"", clientPID, clientCounter);
	                printf(">> Client PID %d connected as \"client0%d\"\n", clientPID, clientCounter);
	                writeToLog(bufferLog);
                }

                else
                {
                	snprintf(bufferLog, sizeof(bufferLog), "Client PID %d connected as \"client0%d\"", clientPID, clientCounter);
                	printf(">> Client PID %d connected as \"client%d\"\n", clientPID, clientCounter);
                	writeToLog(bufferLog);
                }
                clientCounter++;
			}
		}
	}
	close(serverFD);
    close(dummyFD);
    unlink(serverFIFO);

	return EXIT_SUCCESS;
}

void sigint_handler(int signum)
{
	quit = 1;
	terminate_all_childs();
	close_server();
	exit(EXIT_SUCCESS);
}

void setup_signal_handler()
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("Error setup sighandler");
		exit(EXIT_FAILURE);
	}
}

void close_server() 
{
    close(serverFD);
    close(dummyFD);
    unlink(serverFIFO);
}

void terminate_all_childs()
{
	for (int i = 0; i < clientCounter; i++) 
	{
        if (clientInfos[i].pid != 0) 
        {
        	if (kill(clientInfos[i].pid, 0) == 0)
        	{
        		if (kill(clientInfos[i].pid, SIGTERM) == -1)
	            {
	                perror("Failed to send SIGTERM");
	            } 
        	}
            
            if (kill(clientInfosToKill[i].pid, 0) == 0)
        	{
        		if (kill(clientInfosToKill[i].pid, SIGTERM) == -1)
	            {
	                perror("Failed to send SIGTERM");
	            } 
        	}
        }
    }
}
