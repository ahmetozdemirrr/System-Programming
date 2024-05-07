/* neHosClient.c */

#include "systemHelper.h"

/*
	argv[0] -> neHosClient
	argv[1] -> connect type
	argv[2] -> server pid
*/

static char clientFIFO[CLIENT_FIFO_NAME_LEN];
static char serverFIFO[SERVER_FIFO_NAME_LEN];

void setup_signal_handler();
void sigterm_handler(int signum);

int main(int argc, char const *argv[])
{
	if (argc < 3) 
	{
        fprintf(stderr, "Usage: %s <connect type> <server pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	printf("Client pid: %d\n", getpid());
	checkingCommandLineClient(argc, argv[0], argv[1]);
	long int serverPID = (long int)toInteger(argv[2]);

	int serverFD, clientFD, isOK;

	struct Request req;
	struct Response resp;
	char reqInput[COMMUNICATION_LEN];

	setup_signal_handler();

	snprintf(clientFIFO, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
	snprintf(serverFIFO, SERVER_FIFO_NAME_LEN, SERVER_FIFO_TEMPLATE, serverPID);

	if (mkfifo(clientFIFO, 0666) == -1) 
	{
        perror("Failed to create clientFIFO");
        exit(EXIT_FAILURE);
    }
    
	do
	{
		serverFD = open(serverFIFO, O_WRONLY);
	}
	while (serverFD == -1 && errno == EINTR);

	if (serverFD == -1)
	{
		close(serverFD);
		unlink(clientFIFO);
		perror("open error serverFD");
		exit(EXIT_FAILURE);
	}
	req.clientPID = getpid();
	strcpy(req.input, argv[1]);
	
	isOK = write(serverFD, &req, sizeof(struct Request));
	errorExitForSyscall("write error to serverFD", isOK);

	printf(">> Waiting for Que.. Connection established:\n");

	do
	{
		clientFD = open(clientFIFO, O_RDONLY);
	}
	while (clientFD == -1 && errno == EINTR);

	if (clientFD == -1)
	{
		close(clientFD);
		close(serverFD);
		unlink(clientFIFO);
		perror("open error on clientFD");
		exit(EXIT_FAILURE);
	}

	isOK = read(clientFD, &resp, sizeof(struct Response));
	errorExitForSyscall("read error on clientFD", isOK);
	close(clientFD);

	if (resp.seqNum == -1)
	{
		printf("%s\n", resp.output);
		exit(EXIT_SUCCESS);
	}

    while (TRUE)
    {
    	printf(">> Enter comment: ");

    	fgets(reqInput, sizeof(reqInput), stdin);
    	strtok(reqInput, "\n"); // Remove newline

    	int clientWriteFD, clientReadFD;

    	do
		{
			clientWriteFD = open(clientFIFO, O_WRONLY);
		}
		while (clientWriteFD == -1 && errno == EINTR);

		if (clientWriteFD == -1)
		{
			close(clientWriteFD);
			unlink(clientFIFO);
			perror("open error on clientWriteFD");
			exit(EXIT_FAILURE);
		}

		struct Request commandReq;

    	strcpy(commandReq.input, reqInput);
    	commandReq.clientPID = getpid();

    	if (write(clientWriteFD, &commandReq, sizeof(struct Request)) == -1) 
    	{
            perror("write error to clientWriteFD");
            exit(EXIT_FAILURE);
        }
        close(clientWriteFD);

        do
		{
			clientReadFD = open(clientFIFO, O_RDONLY);
		}
		while (clientReadFD == -1 && errno == EINTR);

		if (clientReadFD == -1)
		{
			close(clientReadFD);
			unlink(clientFIFO);
			perror("open error on clientReadFD");
			exit(EXIT_FAILURE);
		}

		struct Response commandResponse;

		memset(&commandResponse, 0, sizeof(struct Response));

        if (read(clientReadFD, &commandResponse, sizeof(struct Response)) == -1) 
        {
            perror("read error on clientReadFD");
            close(clientReadFD);
            exit(EXIT_FAILURE);
        }
        close(clientReadFD);

        if (strcmp(commandResponse.output, "Sending write request to server log file\nwaiting for logfile ...\nlogfile write request granted\nbye..\n") == 0)
        {
        	unlink(clientFIFO);
        	exit(EXIT_SUCCESS);
        }
		printf("\n%s\n", commandResponse.output);
    }
    close(serverFD);
    unlink(clientFIFO);

	return EXIT_SUCCESS;
}

void setup_signal_handler()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No flags

    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("Failed to setup signal handler");
        exit(EXIT_FAILURE);
    }
}

void sigterm_handler(int signum)
{
    printf("SIGTERM received, shutting down...\n");
    unlink(clientFIFO);
    exit(EXIT_SUCCESS);
}
