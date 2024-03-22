#include "cmdHandler.h"
#include "cmdProcess.h"

const char * command1 = "gtuStudentGrades";
const char * command2 = "addStudentGrade";
const char * command3 = "searchStudent";
const char * command4 = "sortAll";
const char * command5 = "showAll";
const char * command6 = "listGrades";
const char * command7 = "listSome";

void logToFile(const char * message)
{
	pid_t childPID = fork();

	if (childPID == -1)
	{
		handleError("Fork error in the logToFile.\n");
	}

	else if (childPID == 0)
	{
		int file = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR | S_IRUSR);

	    if (file == -1)
	    {
	        handleError("Open file error, in the logging function.\n");
	    }

	    time_t now;
	    struct tm * timeInfo;
	    char timeBuffer[TIME_BUFFER];

	    time(&now);

	    timeInfo = localtime(&now);

	    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);

	    write(file, "[", 1);
	    write(file, timeBuffer, strlen(timeBuffer));
	    write(file, "]", 1);
	    write(file, " ", 1);
	    write(file, message, strlen(message));

	    if (close(file) == -1)
	    {
	        char message[MESSAGE_BUFFER];
	    	char * status = "Error: Close file error - ";

	    	strcpy(message, status);
	    	strcat(message, LOG_FILE);
	    	strcat(message, "\n");

	    	handleError(message);
	    }
	    exit(EXIT_SUCCESS);
	}

	else
    {
        int status;

        waitpid(childPID, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            afterWaitErr("logToFile", WEXITSTATUS(status));
        }
    }	
}

void prepareLogFormat(const char * cmdName, const char * param1, const char * param2, const char * param3)
{
	char * status = "Success: "; 
    char * message = " command executed successfully with parameter: ";
    char messBuffer[MESSAGE_BUFFER];

    strcpy(messBuffer, status);
    strcat(messBuffer, cmdName);
    strcat(messBuffer, message);
    strcat(messBuffer, param1);
    strcat(messBuffer, " ");
    strcat(messBuffer, param2);
    strcat(messBuffer, " ");
    strcat(messBuffer, param3);
    strcat(messBuffer, "\n");

    logToFile(messBuffer);
}

void afterWaitErr(const char * funcName, const int errFlag)
{
	char errBuffer[MESSAGE_BUFFER];
	char * message = ": Child process failed with exit status ";
	char errNum[ERR_NUM];

	snprintf(errNum, sizeof(errNum), "%d", errFlag);
	strcpy(errBuffer, funcName);
	strcat(errBuffer, message);
	strcat(errBuffer, errNum);
	strcat(errBuffer, "\n");

	write(STDOUT_FILENO, errBuffer, strlen(errBuffer));        	
	logToFile(errBuffer);
}

void handleError(const char * message)
{
    perror(message);
    logToFile(message);
    exit(EXIT_FAILURE);
}

int endsWithTXT(const char * filename)
{
	const char * extension = ".txt";

	size_t len = strlen(filename);
    size_t extLen = strlen(extension);

    if (len >= extLen && strcmp(filename + len - extLen, extension) == 0) 
    {
        return 1;
    }
    return 0;
}

void readAndTokenizeFile(const char * filename, char tokens[MAX_LINES][2][FBUFFER_SIZE], int * lineIndex)
{
    char fileBuffer[FBUFFER_SIZE];

    int file = open(filename, O_RDONLY);

    if (file == -1)
    {
    	char message[MESSAGE_BUFFER];
    	char * status = "Open file error: ";

    	strcpy(message, status);
    	strcat(message, filename);
    	strcat(message, "\n");

        handleError(message);
    }

    ssize_t readedByte;

    char line[MAX_LINES];
    char * linePtr = line;

    while ((readedByte = read(file, fileBuffer, sizeof(fileBuffer))) > 0)
    {
        for (ssize_t i = 0; i < readedByte; ++i)
        {
            if (fileBuffer[i] == '\n' || i == readedByte - 1)
            {
                *linePtr = '\0'; // Null terminate the line

                char * startQuoteName = strchr(line, '\"');
                char * endQuoteName = strchr(startQuoteName + 1, '\"');
                char * startQuoteGrade = strchr(endQuoteName + 1, '\"');
                char * endQuoteGrade = strchr(startQuoteGrade + 1, '\"');

                if (startQuoteName != NULL && endQuoteName != NULL && startQuoteGrade != NULL && endQuoteGrade != NULL)
                {
                    *endQuoteName = '\0'; // Null terminate the string
                    *endQuoteGrade = '\0'; // Null terminate the string

					// Tırnakları ekleyerek tokenları oluştur
                    strcpy(tokens[*lineIndex][0], "\"");
                    strcpy(tokens[*lineIndex][0] + 1, startQuoteName + 1);
                    strcpy(tokens[*lineIndex][0] + (endQuoteName - startQuoteName), "\"");

                    strcpy(tokens[*lineIndex][1], "\"");
                    strcpy(tokens[*lineIndex][1] + 1, startQuoteGrade + 1);
                    strcpy(tokens[*lineIndex][1] + (endQuoteGrade - startQuoteGrade), "\"");

                    (*lineIndex)++;
                }
                linePtr = line; // Reset line pointer for next line
            }

            else
            {
                *linePtr++ = fileBuffer[i];
            }
        }
    }

    if (readedByte == -1)
    {
        char message[MESSAGE_BUFFER];
    	char * status = "Read file error: ";

    	strcpy(message, status);
    	strcat(message, filename);
    	strcat(message, "\n");

        handleError(message);
    }

    if (close(file) == -1)
    {
        char message[MESSAGE_BUFFER];
    	char * status = "Close file error: ";

    	strcpy(message, status);
    	strcat(message, filename);
    	strcat(message, "\n");

        handleError(message);
    }
}

void sortMenu(int * selection1, int * selection2) 
{
	ssize_t bytesRead;
    char inputBuffer[INPUT_BUFFER_SIZE];

    const char *menuText1 = "Sort by:\n1 - Student\n2 - Grade\n";
    const char *menuText2 = "Sort method:\n1 - Ascending\n2 - Descending\n";

    while (TRUE)
    {
    	write(STDOUT_FILENO, menuText1, strlen(menuText1));

	    bytesRead = read(STDIN_FILENO, inputBuffer, INPUT_BUFFER_SIZE);

	    if (bytesRead == -1) 
	    {
	        perror("Read error!\n");
	        exit(EXIT_FAILURE);
	    } 

	    else if (bytesRead == 0) 
	    {
	        exit(EXIT_SUCCESS);
	    }

	    if (inputBuffer[bytesRead - 1] == '\n') 
	    {
	        inputBuffer[bytesRead - 1] = '\0';
	    }
	    *selection1 = atoi(inputBuffer);

	    if (*selection1 < 1 || *selection1 > 2) 
	    {
	        fprintf(stderr, "Invalid selection for sorting!\n");
	    }

	    else
	    {
	    	break;
	    }
    }
    
    while (TRUE)
    {
    	write(STDOUT_FILENO, menuText2, strlen(menuText2));

	    bytesRead = read(STDIN_FILENO, inputBuffer, INPUT_BUFFER_SIZE);

	    if (bytesRead == -1) 
	    {
	        perror("Read error!\n");
	        exit(EXIT_FAILURE);
	    } 

	    else if (bytesRead == 0) 
	    {
	        exit(EXIT_SUCCESS);
	    }

	    if (inputBuffer[bytesRead - 1] == '\n') 
	    {
	        inputBuffer[bytesRead - 1] = '\0';
	    }
	    *selection2 = atoi(inputBuffer);

	    if (*selection2 < 1 || *selection2 > 2)
	    {
	        fprintf(stderr, "Invalid selection for sorting!\n");
	    }

	    else
	    {
	    	break;
	    }
    }
}

void trimQuotes(char * source) 
{
    size_t len = strlen(source);

    if (len >= 2 && source[0] == '"' && source[len - 1] == '"') 
    {
        char * sourceAfter = source + 1; /* Başındaki tırnağı atlayarak başlangıç adresini ayarla */
        char * dest = source;

        len -= 2; /* Başındaki ve sonundaki tırnakları saymadan uzunluğu ayarla */
        
        while (*sourceAfter && len--) 
        {
            *dest++ = *sourceAfter++; /* Karakteri bir sola kaydır */
        }
        *dest = '\0'; /* Sonlandırıcı karakter ekle */
    }
}

char * mergeStr(const char * str1, const char * str2)
{
	size_t total = strlen(str1) + strlen(str2) + 2; /* +1 for \0 and ' ' character. */

	char * merged = (char *)malloc(total);

	if (merged == NULL)
	{
		handleError("Memory allocation error!\n");
	}
    strcpy(merged, str1);
    strcat(merged, " ");
    strcat(merged, str2);

    return merged;
}

void deleteEnter(char * string)
{
	size_t len = strlen(string);

	if (len > 0 && string[len - 1] == '\n')
	{
		string[len - 1] = '\0';
	}
}

int searchForAdd(const char * filename, const char * target, off_t * byteCount)
{
    char tokens[MAX_LINES][2][FBUFFER_SIZE];
    int lineIndex = 0;

    readAndTokenizeFile(filename, tokens, &lineIndex);

    for (int i = 0; i < lineIndex; ++i)
    {
        if (strcmp(target, tokens[i][0]) == 0)
        {
            return 1;
        }

        else
        {
        	*byteCount += strlen(tokens[i][0]) + strlen(tokens[i][1]) + 2;
        }
    }
    return -1;
}

void process(int tokenCount, char * tokens[], int maxTokens)
{
	if (tokenCount > 0)
	{
		/*
			The fgets function does not ignore the \n character at the end of the line when 
			receiving input. We ignore that part here for all commands
		*/
		deleteEnter(tokens[0]);

		if (strcmp(tokens[0], command1) == 0) /* Handle command 1 (gtuStudentGrades) */
		{
			if (tokenCount == 1) /* when just gtuStudentGrades command is entered */ 
			{
				logToFile("User entered command: gtuStudentGrades\n");
				manGTUCMD();
			}

			else if (tokenCount == 2)
			{
				deleteEnter(tokens[1]);
				logToFile("User entered command: gtuStudentGrades\n");
				gtuStudentGradesCMD(tokens[1]);
			}
			
			else 
			{
				fprintf(stderr, "Too many arguments.\nUsage: gtuStudentGrades or gtuStudentGrades <filename>\n");
			}
		}

		else if (strcmp(tokens[0], command2) == 0) /* Handle command 2 (addStudentGrade) */
		{
			if (tokenCount == 4 && tokens[1] != NULL && tokens[2] != NULL && tokens[3] != NULL)
			{
				int isFit = 0;
				char * gradeFormat[10] = {"\"AA\"", "\"BA\"", "\"BB\"", "\"CB\"", "\"CC\"", "\"DC\"", "\"DD\"", "\"FF\"", "\"VF\"", "\"NA\""};

				for (int i = 0; i < 10; ++i)
				{
					if (strcmp(tokens[2], gradeFormat[i]) == 0)
					{
						isFit = 1;
					}
				}

				if (isFit)
				{
					logToFile("User entered command: addStudentGrade\n");
					addStudentGradeCMD(tokens[1], tokens[2], tokens[3]);
				}

				else
				{
					fprintf(stderr, "Only AA, BA, BB, CB, CC, DC, DD, FF, VF, NA format is acceptable for grade.\n");
				}
			}

			else if (tokenCount < 4)
			{
				fprintf(stderr, "Too few arguments.\nUsage: addStudentGrade \"Name Surname\" \"Grade\" \"<filename>\"\n");
			}

			else
			{
				fprintf(stderr, "Too many arguments.\nUsage: addStudentGrade \"Name Surname\" \"Grade\" \"<filename>\"\n");
			}
		}

		else if (strcmp(tokens[0], command3) == 0) /* Handle command 3 (searchStudent) */
		{
			if (tokenCount == 3 && tokens[1] != NULL && tokens[2] != NULL)
			{
				logToFile("User entered command: searchStudent\n");
				searchStudentCMD(tokens[1], tokens[2]);
			}

			else if (tokenCount < 3)
			{
				fprintf(stderr, "Too few arguments.\nUsage: searchStudent \"Name Surname\" \"<filename>\"\n");
			}

			else
			{
				fprintf(stderr, "Too many arguments.\nUsage: searchStudent \"Name Surname\" \"<filename>\"\n");
			}
		}

		else if (strcmp(tokens[0], command4) == 0) /* Handle command 4 (sortAll) */
		{
			if (tokenCount == 2 && tokens[1] != NULL)
			{
				logToFile("User entered command: sortAll\n");
				sortAllCMD(tokens[1]);
			}

			else if (tokenCount < 2)
			{
				fprintf(stderr, "Too few arguments.\nUsage: sortAll \"<filename>\"\n");
			}

			else
			{
				fprintf(stderr, "Too many arguments.\nUsage: sortAll \"<filename>\"\n");
			}
		}

		else if (strcmp(tokens[0], command5) == 0) /* Handle command 5 (showAll) */
		{
			if (tokenCount == 2 && tokens[1] != NULL)
			{
				logToFile("User entered command: showAll\n");
				showAllCMD(tokens[1]);
			}

			else if (tokenCount < 2)
			{
				fprintf(stderr, "Too few arguments.\nUsage: showAll \"<filename>\"\n");
			}

			else
			{
				fprintf(stderr, "Too many arguments.\nUsage: showAll \"<filename>\"\n");
			}
		}

		else if (strcmp(tokens[0], command6) == 0) /* Handle command 6 (listGrades) */
		{
			if (tokenCount == 2 && tokens[1] != NULL)
			{
				logToFile("User entered command: listGrades\n");
				listGradesCMD(tokens[1]);
			}

			else if (tokenCount < 2)
			{
				fprintf(stderr, "Too few arguments.\nUsage: listGrades \"<filename>\"\n");
			}

			else
			{
				fprintf(stderr, "Too many arguments.\nUsage: listGrades \"<filename>\"\n");
			}			 
		}

		else if (strcmp(tokens[0], command7) == 0) /* Handle command 7 (listSome) */
		{
			if (tokenCount == 4 && tokens[1] != NULL && tokens[2] != NULL && tokens[3] != NULL)
			{
				const char * message = "Enter a valid number. Example input: listSome 5 2 \"grades.txt\"\n\n";

				if (strlen(tokens[1]) == 1 && tokens[1][0] == 48)
				{					
					write(STDOUT_FILENO, message, strlen(message));
				}
				
				else
				{
					for (int i = 0 ; i < strlen(tokens[1]); ++i)
					{
						if (tokens[1][i] < 48 || tokens[1][i] > 57)
						{
							write(STDOUT_FILENO, message, strlen(message));			
							break;
						}
					
					}
				}
				int pageNumberAmount = atoi(tokens[1]); //kontrol koy
				int pageNumberIndex = atoi(tokens[2]);

				if (pageNumberAmount < 1 || pageNumberIndex < 0)
				{
					fprintf(stderr, "Error: Enter a valid number for page amount and page index.\n");
				}

				else
				{
					logToFile("User entered command: listSome\n");
					listSomeCMD(tokens[3], pageNumberAmount, pageNumberIndex);
				}
			}

			else if (tokenCount < 4)
			{
				fprintf(stderr, "Too few arguments.\nUsage: listSome \"<filename>\" or listSome number number \"<filename>\"\n");
			}

			else
			{
				fprintf(stderr, "Too many arguments.\nUsage: listSome \"<filename>\" or listSome number number \"<filename>\"\n");
			}	
		}

		else /* Handle error */
		{
			fprintf(stderr, "%s: command not found\n", tokens[0]);
		}
	}
}
