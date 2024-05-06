/* systemHelper.c */

#include "systemHelper.h"

sem_t * logSem; /* semaphore for logging process */
static int logFD;

void initLog(const char * semName, const char * logFileName) 
{
    logSem = sem_open(semName, O_CREAT, 0644, 1);

    if (logSem == SEM_FAILED) 
    {
        perror("Failed to open semaphore");
        exit(EXIT_FAILURE);
    }

    logFD = open(logFileName, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if (logFD == -1) 
    {
        perror("Failed to open log file");
        sem_close(logSem);
        sem_unlink(semName);
        exit(EXIT_FAILURE);
    }
}

void writeToLog(const char * message) 
{
    char logEntry[1024];
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);

    snprintf(logEntry, sizeof(logEntry), "%d-%02d-%02d %02d:%02d:%02d - %s\n",
             timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
             timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);

    sem_wait(logSem);
    write(logFD, logEntry, strlen(logEntry));
    sem_post(logSem);
}

void closeLog(const char * semName) 
{
    close(logFD);
    sem_close(logSem);
    sem_unlink(semName);
}

void errorExitForSyscall(const char * errorMessage, int flag)
{
	if (flag == -1)
	{
		perror(errorMessage);
		exit(EXIT_FAILURE);
	}
}

void checkingCommandLineServer(int numOfInputs, const char * command)
{
	if (strcmp(command, "./neHosServer") != 0)
	{
		const char * errorMessage = "Invalid command: enter only neHosServer\n";
		
		int isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineServer function", isWrite);

		exit(EXIT_FAILURE);
	}

	if (numOfInputs < 3)
	{
		const char * errorMessage = "To few arguments, usage: neHosServer <dirname> <max. #ofClients>\n";
		
		int isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineServer function", isWrite);

		exit(EXIT_FAILURE);
	}

	else if (numOfInputs > 3)
	{
		const char * errorMessage = "To many arguments, usage: neHosServer <dirname> <max. #ofClients>\n";
		
		int isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineServer function", isWrite);

		exit(EXIT_FAILURE);
	}
}

void checkingCommandLineClient(int numOfInputs, const char * command, const char * connectType)
{
	int isWrite;

	if (strcmp(command, "./neHosClient") != 0)
	{
		const char * errorMessage = "Invalid command: enter only neHosServer\n";
		
		isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineClient function", isWrite);

		exit(EXIT_FAILURE);
	}

	if ((strcmp(connectType, "Connect") != 0) && (strcmp(connectType, "tryConnect") != 0))
	{
		const char * errorMessage = "Invalid type of connect: enter only Connect or tryConnect\n";
		
		isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineClient function", isWrite);

		exit(EXIT_FAILURE);
	}

	if (numOfInputs < 3)
	{
		const char * errorMessage = "To few arguments, usage: neHosClient <Connect/tryConnect> ServerPID\n";
		
		isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineClient function", isWrite);

		exit(EXIT_FAILURE);
	}

	else if (numOfInputs > 3)
	{
		const char * errorMessage = "To many arguments, usage: neHosClient <Connect/tryConnect> ServerPID\n";
		
		isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
		errorExitForSyscall("Write error on checkingCommandLineClient function", isWrite);

		exit(EXIT_FAILURE);
	}
}

uint32_t toInteger(const char * toNumber)
{
	char * endptr;
	/* I use strtol to minimise error cases */
	uint32_t result = strtol(toNumber, &endptr, NUM_BASE_SYSTEM); /* NUM_BASE_SYSTEM = 10 */
	
	if (*endptr == '\0')
	{
		return result;
	}
	const char * errorMessage = "Invalid input for number of clients!\n";
	
	int isWrite = write(STDERR_FILENO, errorMessage, strlen(errorMessage));
	errorExitForSyscall("Write error on toInteger function", isWrite);

	return 0;
}

void takeClientCommands(const char * clientFIFO, const char * directoryPath, DIR * CWdirectory, pid_t clientPID)
{
	struct Request req;
	struct Response resp;

	while (TRUE)
    {
    	int fifoReadFD = open(clientFIFO, O_RDONLY);
		errorExitForSyscall("Client fifo open error on takeClientCommands function", fifoReadFD);

        ssize_t bytesRead = read(fifoReadFD, &req, sizeof(struct Request));

        if (bytesRead == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                continue;
            }

            else
            {
                perror("Failed to read from client FIFO");
                break;
            }
        }
        
        else if (bytesRead == 0)
        {
            break;
        }    
		close(fifoReadFD);

		int fifoWriteFD = open(clientFIFO, O_WRONLY);
		errorExitForSyscall("Client fifo open error on takeClientCommands function", fifoWriteFD);

		if (bytesRead > 0 && req.clientPID > 0)
		{
			memset(&resp, 0, sizeof(struct Response));
	        strcpy(resp.output, handleClients(&req, directoryPath, CWdirectory, clientPID));
	    	resp.seqNum = 1111;

	    	if (write(fifoWriteFD, &resp, sizeof(struct Response)) == -1)
	        {
	            perror("Failed to write to client FIFO");
	            break;
	        }

	        if (strcmp(resp.output, "Sending write request to server log file\nwaiting for logfile ...\nlogfile write request granted\nbye..\n") == 0)
	    	{
	    		//kill(getpid(), SIGINT);
	    	    break;
	    	}			
		}
		close(fifoWriteFD);
    }
}

char * handleClients(struct Request * req, const char * directoryPath, DIR * CWdirectory, pid_t clientPID)
{
	static char response[COMMUNICATION_LEN];
	memset(response, 0, sizeof(response));

	char claRequest[4][30];

	char * commAndParams = strtok(req->input, " ");

	int countParam = 0;

	while (commAndParams != NULL && countParam < 4)
	{
		strcpy(claRequest[countParam], commAndParams);
		commAndParams = strtok(NULL, " ");
		countParam++;
	}

	if (strcmp(claRequest[0], COMMAND_1) == 0) /* help command */
	{
		if (countParam == 1)
		{
			const char * serversCommandInfo = "Available comments are :\n\nhelp, list, readF, writeT, upload, download, archServer,\nquit, killServer";
			strcpy(response, serversCommandInfo);
		}
		
		else if (countParam == 2)
		{
			strcpy(response, commandsMAN(claRequest[1]));
		}

		else
		{
			strcpy(response, errorForParam(claRequest[0]));
		}
	}

	else if (strcmp(claRequest[0], COMMAND_2) == 0) /* list command */
	{
		if (countParam == 1)
		{
			strcpy(response, listCWD(CWdirectory));
		}

		else
		{
			strcpy(response, errorForParam(claRequest[0]));
		}
	}

	else if (strcmp(claRequest[0], COMMAND_3) == 0) /* readF command */
	{
		if (countParam == 2)
		{
			strcpy(response, readThisFile(claRequest[1], FALSE, directoryPath)); /* indeks of file line must be > 0 */
		}

		else if (countParam == 3)
		{
			strcpy(response, readThisFile(claRequest[1], toInteger(claRequest[2]), directoryPath));
		}

		else
		{
			strcpy(response, errorForParam(claRequest[0]));
		}
	}

	else if (strcmp(claRequest[0], COMMAND_4) == 0) /* writeT command */
	{
		if (countParam == 3)
        {
            strcpy(response, writeToFile(claRequest[1], FALSE, directoryPath, claRequest[2]));
        }

        else if (countParam == 4)
        {
            strcpy(response, writeToFile(claRequest[1], toInteger(claRequest[2]), directoryPath, claRequest[3]));
        }

        else
        {
            strcpy(response, errorForParam(claRequest[0]));
        }
	}

	else if (strcmp(claRequest[0], COMMAND_5) == 0) /* upload command */
	{
		if (countParam == 2)
		{
			strcpy(response, "file transfer request received. Beginning file transfer:\n");

			int transferredByte = upload(directoryPath, claRequest[1]);

			if (transferredByte == -1)
			{
				strcat(response, "upload commend is failed!\n");
			}

			else
			{
				char temp[128];
				snprintf(temp, sizeof(temp), "%d bytes transferred\n", transferredByte);
				strcat(response, temp);
			}
		}

		else
		{
			strcpy(response, errorForParam(claRequest[0]));
		}
	}

	else if (strcmp(claRequest[0], COMMAND_6) == 0) /* download command */
	{
	    if (countParam == 2)
	    {
	        strcpy(response, "File download request received. Starting download:\n");

	        int transferredBytes = download(directoryPath, claRequest[1]);

	        if (transferredBytes == -1)
	        {
	            strcat(response, "Download command failed!\n");
	        }

	        else
	        {
	            char temp[128];
	            snprintf(temp, sizeof(temp), "%d bytes downloaded.\n", transferredBytes);
	            strcat(response, temp);
	        }
	    }

	    else
	    {
	        strcpy(response, errorForParam(claRequest[0]));
	    }
	}

	else if (strcmp(claRequest[0], COMMAND_7) == 0) /* archServer command */
	{
	    if (countParam == 2)
	    {
	        char tarCommand[4096];
	        char tempArchivePath[1024];
	        char finalArchivePath[2048];
	        int fileCount = 0;
	        int totalBytes = 0;
	        struct dirent *entry;
	        DIR * dir = opendir(directoryPath);

	        if (dir == NULL) 
	        {
	            strcpy(response, "Error opening directory.\n");
	            return response;
	        }

	        while ((entry = readdir(dir)) != NULL) 
	        {
	            if (entry->d_type == DT_REG) 
	            {
	                fileCount++;
	                char filePath[1024];
	                snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
	                struct stat statbuf;

	                if (stat(filePath, &statbuf) == 0)
	                {
	                    totalBytes += statbuf.st_size;
	                }
	            }
	        }
	        closedir(dir);

	        snprintf(tempArchivePath, sizeof(tempArchivePath), "/tmp/%s", claRequest[1]);
	        snprintf(finalArchivePath, sizeof(finalArchivePath), "%s/%s", directoryPath, claRequest[1]);
	        snprintf(tarCommand, sizeof(tarCommand), "tar -cf %s -C %s .", tempArchivePath, directoryPath);
	        snprintf(response, sizeof(response), "Archiving the current contents of the server...\ncreating archive directory ...\n%d files downloaded ..%d bytes transferred ..\nCalling tar utility .. child PID %d\n", fileCount, totalBytes, getpid());

	        pid_t childPID = fork();

	        if (childPID == 0)
	        {
	            execl("/bin/sh", "sh", "-c", tarCommand, (char *)NULL);
	            exit(EXIT_FAILURE);
	        }

	        else if (childPID < 0)
	        {
	            strcat(response, "Failed to fork process.\n");
	        }

	        else
	        {
	            int status;
	            waitpid(childPID, &status, 0);

	            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	            {
	                snprintf(tarCommand, sizeof(tarCommand), "mv %s %s", tempArchivePath, finalArchivePath);
	                system(tarCommand);

	                strcat(response, "child returned with SUCCESS..\ncopying the archive file..\nremoving archive directory...\n");
	                snprintf(tarCommand, sizeof(tarCommand), "SUCCESS Server side files are archived in \"%s\"\n", finalArchivePath);
	                strcat(response, tarCommand);
	            }

	            else
	            {
	                strcat(response, "Failed to create archive.\n");
	            }
	        }
	    }

	    else
	    {
	        strcpy(response, errorForParam(claRequest[0]));
	    }
	}

	else if (strcmp(claRequest[0], COMMAND_8) == 0) /* killServer command */
	{
		kill(getppid(), SIGINT);
	}

	else if (strcmp(claRequest[0], COMMAND_9) == 0) /* quit command */
	{
		char bufferLog[128];
		snprintf(bufferLog, sizeof(bufferLog), "Quit command received PID: %d, logging and terminating client session.", clientPID);
		writeToLog(bufferLog);
        snprintf(response, COMMUNICATION_LEN, "Sending write request to server log file\nwaiting for logfile ...\nlogfile write request granted\nbye..\n");
	}

	else
	{
		strcpy(response, "Command not found\n");
	}
	return response;
}

char * errorForParam(const char * command)
{
	const char * errorMessageMissArg = ": missing or to many operand\0";
	char * errorMessageComplete = NULL;

	strcpy(errorMessageComplete, command);
	strcat(errorMessageComplete, errorMessageMissArg);

	return errorMessageComplete;
}

char * commandsMAN(char * command)
{
	if (strcmp(command, COMMAND_1) == 0)
	{
		return "help \n\tdisplay the list of possible client requests\0";
	}

	else if (strcmp(command, COMMAND_2) == 0)
	{
        return "list\n\tsends a request to display the list of files in Servers directory\n\t(also displays the list received from the Server)\0";
	}

	else if (strcmp(command, COMMAND_3) == 0)
	{
        return "readF <file> <line #>\n\trequests to display the # line of the <file>, if no line number is given\n\tthe whole contents of the file is requested (and displayed on the client side)\0";
	}

	else if (strcmp(command, COMMAND_4) == 0)
	{
        return "writeT <file> <line #> <string>\n\trequest to write the content of “string” to the #th line the <file>, if the line # is not given\n\twrites to the end of file. If the file does not exists in Servers directory creates and edits the\n\tfile at the same time\0";
	}

	else if (strcmp(command, COMMAND_5) == 0)
	{
        return "upload <file>\n\tuploads the file from the current working directory of client to the Servers directory\n\t(beware of the cases no file in clients current working directory and file with the same\n\tname on Servers side)\0";
	}

	else if (strcmp(command, COMMAND_6) == 0)
	{
        return "download <file>\n\trequest to receive <file> from Servers directory to client side\0";
	}

	else if (strcmp(command, COMMAND_7) == 0)
	{
        return "archServer <fileName>.tar\n\tUsing fork, exec and tar utilities create a child process that will collect all the files currently\n\tavailable on the the Server side and store them in the <filename>.tar archive\0";
	}

	else if (strcmp(command, COMMAND_8) == 0)
	{
        return "killServer\n\tSends a kill request to the Server\0";
	}

	else if (strcmp(command, COMMAND_9) == 0)
	{
        return "quit\n\tSend write request to Server side log file and quits\0";
	}

	else
	{
		char * toReturn = NULL;
		const char * error = ": command not found\0";
		strcpy(toReturn, command);
		strcat(toReturn, error);

		return toReturn; 
	}
}

char * listCWD(DIR * serverDir)
{
	struct dirent * dentry;
	static char response[COMMUNICATION_LEN];
	memset(response, 0, sizeof(response));

	rewinddir(serverDir); /* Reset the position of the directory stream serverDir to the beginning of the directory */

	response[0] = '\0';

	while ((dentry = readdir(serverDir)) != NULL)
	{
		if (strcmp(dentry->d_name, ".") != 0 && strcmp(dentry->d_name, "..") != 0)
		{
			strcat(response, dentry->d_name);
			strcat(response, "\n");
		}
	}
	return response;
}

char *readThisFile(const char *filename, int lineIndex, const char *directoryPath) {
    if (lineIndex < 1) {
        return "Line index must be a positive number.";
    }

    char fullPath[COMMUNICATION_LEN];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", directoryPath, filename);

    int fileDesc = open(fullPath, O_RDONLY);
    if (fileDesc == -1) {
        perror("Failed to open file");
        static char errorResponse[COMMUNICATION_LEN];
        snprintf(errorResponse, COMMUNICATION_LEN, "Error opening file: %s", strerror(errno));
        return errorResponse;
    }

    static char lineBuffer[COMMUNICATION_LEN];
    memset(lineBuffer, 0, sizeof(lineBuffer));
    char ch;
    int bytesRead;
    int currentLine = 1;
    int index = 0;

    while ((bytesRead = read(fileDesc, &ch, 1)) > 0) {
        if (ch != '\n') {
            if (index < COMMUNICATION_LEN - 1) {
                lineBuffer[index++] = ch;
            }
        } else {
            lineBuffer[index] = '\0'; // Null terminate the current line
            if (currentLine == lineIndex) {
                close(fileDesc);
                return lineBuffer;
            }
            index = 0; // Reset index for the next line
            currentLine++;
        }
    }

    close(fileDesc);
    if (bytesRead == -1) {
        perror("Error reading file");
        static char errorResponse[COMMUNICATION_LEN];
        snprintf(errorResponse, COMMUNICATION_LEN, "Error reading file: %s", strerror(errno));
        return errorResponse;
    }

    if (lineIndex > currentLine) {
        static char lineError[COMMUNICATION_LEN];
        snprintf(lineError, COMMUNICATION_LEN, "Invalid index for file line: %d", lineIndex);
        return lineError;
    }

    return "Line not found.";
}


char * writeToFile(const char * filename, int lineIndex , const char * directoryPath, const char * text)
{
	static char response[COMMUNICATION_LEN];
    char fullPath[COMMUNICATION_LEN];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", directoryPath, filename);

    int fileDesc = open(fullPath, O_RDWR | O_CREAT, 0644);

    if (fileDesc == -1) 
    {
        snprintf(response, sizeof(response), "Error opening file: %s", strerror(errno));
        return response;
    }

    char * buffer = malloc(COMMUNICATION_LEN * 10);

    if (!buffer) 
    {
        close(fileDesc);
        return "Memory allocation failed for buffer";
    }

    char * temp = buffer;
    ssize_t bytesRead, totalBytes = 0;

    while ((bytesRead = read(fileDesc, temp, COMMUNICATION_LEN - 1)) > 0) 
    {
        temp += bytesRead;
        totalBytes += bytesRead;
    }

    if (bytesRead == -1) 
    {
        snprintf(response, sizeof(response), "Error reading file: %s", strerror(errno));
        free(buffer);
        close(fileDesc);
        return response;
    }
    *temp = '\0';

    char * newContent = malloc(COMMUNICATION_LEN * 10);

    if (!newContent) 
    {
        free(buffer);
        close(fileDesc);
        return "Memory allocation failed for new content";
    }

    char * line = strtok(buffer, "\n");
    int currentLine = 1;
    size_t newContentLength = 0;
    int lineWritten = 0;

    while (line) 
    {
        if (currentLine == lineIndex) 
        {
            newContentLength += snprintf(newContent + newContentLength, COMMUNICATION_LEN, "%s\n", text);
            lineWritten = 1;
        }
        newContentLength += snprintf(newContent + newContentLength, COMMUNICATION_LEN, "%s\n", line);
        line = strtok(NULL, "\n");
        currentLine++;
    }

    if (lineIndex == 0 || !lineWritten) 
    {
        snprintf(newContent + newContentLength, COMMUNICATION_LEN, "%s\n", text);
    }

    if (lseek(fileDesc, 0, SEEK_SET) == -1) 
    {
        snprintf(response, sizeof(response), "Error seeking in file: %s", strerror(errno));
        free(buffer);
        free(newContent);
        close(fileDesc);
        return response;
    }

    if (write(fileDesc, newContent, strlen(newContent)) < strlen(newContent)) 
    {
        snprintf(response, sizeof(response), "Error writing to file: %s", strerror(errno));
        free(buffer);
        free(newContent);
        close(fileDesc);
        return response;
    }

    snprintf(response, sizeof(response), "Write operation successful");
    free(buffer);
    free(newContent);
    close(fileDesc);
    return response;
}

int upload(const char * directoryPath, const char * filename) 
{
    int sourceFd, destFd, shmFd;
    void *shmAddr;
    struct stat statbuf;
    size_t fileSize, remainingSize, chunkSize, totalWritten = 0;
    char sourcePath[BUFFER_SIZE], destPath[BUFFER_SIZE];

    snprintf(sourcePath, sizeof(sourcePath), "../midterm/%s", filename);
    snprintf(destPath, sizeof(destPath), "%s/%s", directoryPath, filename);

    sourceFd = open(sourcePath, O_RDONLY);

    if (sourceFd == -1) 
    {
        perror("Open error sourcePath");
        return -1;
    }

    if (fstat(sourceFd, &statbuf) == -1) 
    {
        perror("Stat call failed");
        close(sourceFd);
        return -1;
    }
    fileSize = statbuf.st_size;
    remainingSize = fileSize;

    shmFd = shm_open("/myShm", O_CREAT | O_RDWR, 0666);

    if (shmFd == -1) 
    {
        perror("cannot open shared memory");
        close(sourceFd);
        return -1;
    }

    if (ftruncate(shmFd, fileSize) == -1) 
    {
        perror("Ftruncate call failed");
        close(sourceFd);
        shm_unlink("/myShm");
        close(shmFd);
        return -1;
    }

    shmAddr = mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    if (shmAddr == MAP_FAILED) 
    {
        perror("Memory mapping failed");
        close(sourceFd);
        shm_unlink("/myShm");
        close(shmFd);
        return -1;
    }

    while (remainingSize > 0) 
    {
        chunkSize = read(sourceFd, (char *) shmAddr + totalWritten, BUFFER_SIZE);

        if (chunkSize == -1) 
        {
            perror("read file error chunkSize");
            munmap(shmAddr, fileSize);
            close(sourceFd);
            shm_unlink("/myShm");
            close(shmFd);
            return -1;
        }
        remainingSize -= chunkSize;
        totalWritten += chunkSize;
    }

    destFd = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (destFd == -1) 
    {
        perror("target directory cannot open or file already exists");
        munmap(shmAddr, fileSize);
        close(sourceFd);
        shm_unlink("/myShm");
        close(shmFd);
        return -1;
    }

    if (write(destFd, shmAddr, fileSize) != fileSize)
    {
        perror("write error to target directory");
        munmap(shmAddr, fileSize);
        close(sourceFd);
        close(destFd);
        shm_unlink("/myShm");
        close(shmFd);
        return -1;
    }
    munmap(shmAddr, fileSize);
    close(sourceFd);
    close(destFd);
    shm_unlink("/myShm");
    close(shmFd);

    return totalWritten;
}

int download(const char * directoryPath, const char * filename) 
{
    int sourceFd, destFd;
    size_t bytesRead, bytesWritten, totalBytes = 0;
    char buffer[BUFFER_SIZE];
    char sourcePath[BUFFER_SIZE], destPath[BUFFER_SIZE];

    snprintf(sourcePath, sizeof(sourcePath), "%s/%s", directoryPath, filename);
    sourceFd = open(sourcePath, O_RDONLY);

    if (sourceFd == -1) 
    {
        perror("Failed to open source file");
        return -1;
    }

    snprintf(destPath, sizeof(destPath), "../midterm/%s", filename);
    destFd = open(destPath, O_WRONLY | O_CREAT | O_EXCL, 0666);

    if (destFd == -1) 
    {
        if (errno == EEXIST) 
        {
            perror("Destination file already exists");
        } 

        else 
        {
            perror("Failed to open or create destination file");
        }
        close(sourceFd);
        return -1;
    }

    while ((bytesRead = read(sourceFd, buffer, BUFFER_SIZE)) > 0) 
    {
        bytesWritten = write(destFd, buffer, bytesRead);

        if (bytesWritten != bytesRead) 
        {
            perror("Failed to write to destination file");
            close(sourceFd);
            close(destFd);
            return -1;
        }
        totalBytes += bytesWritten;
    }

    if (bytesRead == -1) 
    {
        perror("Error reading source file");
        close(sourceFd);
        close(destFd);
        return -1;
    }
    close(sourceFd);
    close(destFd);
    return totalBytes;
}
