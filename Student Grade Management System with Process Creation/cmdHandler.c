#include "cmdHandler.h"
#include "cmdProcess.h"

void manGTUCMD()
{
	pid_t childPID = fork();

	if (childPID == -1)
	{
		handleError("Fork error in the manGTUCMD.\n");
	}

	else if (childPID == 0)
	{
		/* const char * red = "\033[0;31m"; */
		/* const char * reset = "\033[0m"; */
		const char * command1 = "gtuStudentGrades: lists all commands in the system.\n";
		/*const char * command11 = ": lists all commands in the system.\n";*/
		const char * command2 = "gtuStudentGrades <filename>: opens a file on the system with the given file name.\n";
		/*const char * command22 = ": opens a file on the system with the given file name.\n";*/
		const char * command3 = "addStudentGrade \"name surname\" \"grade\" <filename>: a student is added to the filename file with the given information.\n";
		/*const char * command33 = ": a student is added to the filename file with the given information.\n";*/
		const char * command4 = "searchStudent \"name surname\" <filename>: checks the filename file for the given name and prints detailed information about the name if it exists.\n";
		/*const char * command44 = ": checks the filename file for the given name and prints detailed information about the name if it exists.\n";*/
		const char * command5 = "sortAll <filename>: sorts the lines in the file in the desired format and prints them on the screen.\n";
		/*const char * command55 = ": sorts the lines in the file in the desired format and prints them on the screen.\n";*/
		const char * command6 = "showAll <filename>: lists all content of the filename.\n";
		/*const char * command66 = ": lists all content of the filename.\n";*/
		const char * command7 = "listGrades <filename>: lists first five elements in the filename.\n";
		/*const char * command77 = ": lists first five elements in the filename.\n";*/
		const char * command8 = "listSome page-amount page-index <filename>: divides the file into pages according to the given page-amount number and prints the information on the desired page to the screen with page-index.\n";
		/*const char * command88 = ": divides the file into pages according to the given page-amount number and prints the information on the desired page to the screen with page-index.\n";*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command1, strlen(command1));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command11, strlen(command11));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command2, strlen(command2));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command22, strlen(command22));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command3, strlen(command3));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command33, strlen(command33));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command4, strlen(command4));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command44, strlen(command44));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command5, strlen(command5));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command55, strlen(command55));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command6, strlen(command6));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command66, strlen(command66));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command7, strlen(command7));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command77, strlen(command77));*/

		/*write(STDOUT_FILENO, red, strlen(red));*/
		write(STDOUT_FILENO, command8, strlen(command8));
		/*write(STDOUT_FILENO, reset, strlen(reset));*/
		/*write(STDOUT_FILENO, command88, strlen(command88));*/
		logToFile("Success: gtuStudentGrades command executed successfully.\n");
		exit(EXIT_SUCCESS);
	}
	
	else
    {
        int status;

        waitpid(childPID, &status, 0); /* wait for the child to complete the process */

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) /* to prevent possible zombie processes */
        {
        	afterWaitErr("manGTUCMD", WEXITSTATUS(status));
        }
    }
}

void gtuStudentGradesCMD(const char * filename)
{
    pid_t childPID = fork();

    if (childPID == -1)
    {
        handleError("Fork error in the gtuStudentGradesCMD.\n");
    }

    else if (childPID == 0)
    {
        int file = open(filename, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR); /* Yazma-okuma yetkisi olacak bir dosya aç, yoksa oluştur. */

        if (file == -1)	
        {
            char message[MESSAGE_BUFFER];
	    	char * status = "Error: Open file error - ";

	    	strcpy(message, status);
	    	strcat(message, filename);
	    	strcat(message, "\n");

	        handleError(message);
        }

        if (close(file) == -1)
        {
            char message[MESSAGE_BUFFER];
	    	char * status = "Error: Close file error - ";

	    	strcpy(message, status);
	    	strcat(message, filename);
	    	strcat(message, "\n");

        	handleError(message);
        }
        prepareLogFormat("gtuStudentGrades", filename, "", "");
        exit(EXIT_SUCCESS);
    }

    else
    {
        int status;

        waitpid(childPID, &status, 0); /* wait for the child to complete the process */

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) /* to prevent possible zombie processes */
        {
        	afterWaitErr("gtuStudentGradesCMD", WEXITSTATUS(status));
        }
    }
}

void addStudentGradeCMD(const char * nameSurname, const char * grade, char * filename)
{
    pid_t childPID = fork();

    if (childPID == -1)
    {
        handleError("Fork error in the addStudentGradeCMD.\n");
    }

    else if (childPID == 0)
    {
    	deleteEnter(filename);
    	off_t byteCount = 0;

    	char tokens[MAX_LINES][2][FBUFFER_SIZE];
	    int lineIndex = 0, existingİndex = 0;

	    readAndTokenizeFile(filename, tokens, &lineIndex);
	    
	    for (int i = 0; i < lineIndex; ++i)
	    {
	        if (strcmp(nameSurname, tokens[i][0]) == 0)
	        {
	        	existingİndex = 1;
	            break;
	        }

	        else
	        {
	        	byteCount += strlen(tokens[i][0]) + strlen(tokens[i][1]) + 2;
	        }
	    }

	    int file = open(filename, O_WRONLY, S_IWUSR | S_IRUSR);

        if (file == -1)
        {
            char message[MESSAGE_BUFFER];
	    	char * status = "Error: Open file error - ";

	    	strcpy(message, status);
	    	strcat(message, filename);
	    	strcat(message, "\n");

	        handleError(message);
        }

        char * merged = mergeStr(nameSurname, grade);

        if (merged == NULL)
        {
            handleError("Error: Memory allocation error, in the addStudentGrade function.\n");
        }
        strcat(merged, "\n"); /* dosyaya yazıldıktan sonra yeni satıra geçmesini sağlıyoruz. */
        size_t strlength = strlen(merged);

    	if (existingİndex == 0)
    	{
    		lseek(file, 0, SEEK_END);
	        ssize_t writtenBytes = write(file, merged, strlength);

	        if (writtenBytes < strlength)
	        {
	            char message[MESSAGE_BUFFER];
		    	char * status = "Error: Write file error - ";

		    	strcpy(message, status);
		    	strcat(message, filename);
		    	strcat(message, "\n");

		        handleError(message);
	        }
	        free(merged);
	        prepareLogFormat("addStudentGrade", nameSurname, grade, filename);
    	}

    	else
    	{	
    		lseek(file, byteCount, SEEK_SET); /* imleci güncellenecek satıra getiriyoruz. */

    		ssize_t writtenBytes = write(file, merged, strlength);

	        if (writtenBytes < strlength)
	        {
	            char message[MESSAGE_BUFFER];
		    	char * status = "Error: Write file error - ";

		    	strcpy(message, status);
		    	strcat(message, filename);
		    	strcat(message, "\n");

		        handleError(message);
	        }
	        free(merged);
	        prepareLogFormat("addStudentGrade update grade", nameSurname, grade, filename);

	        lseek(file, 0, SEEK_END);
    	}

    	if (close(file) == -1)
        {
            char message[MESSAGE_BUFFER];
	    	char * status = "Error: Close file error - ";

	    	strcpy(message, status);
	    	strcat(message, filename);
	    	strcat(message, "\n");

	        handleError(message);
        }
        exit(EXIT_SUCCESS);
    }

    else
    {
        int status;

        waitpid(childPID, &status, 0); /* wait for the child to complete the process */

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) /* to prevent possible zombie processes */
        {
            afterWaitErr("addStudentGradeCMD", WEXITSTATUS(status));
        }
    }
}

void searchStudentCMD(const char * target, const char * filename)
{
    pid_t childPID = fork();

    if (childPID == -1)
    {
        handleError("Fork error in the searchStudentCMD.\n");
    }

    else if (childPID == 0)
    {
        char tokens[MAX_LINES][2][FBUFFER_SIZE];
        int lineIndex = 0, foundFlag = 0;

        readAndTokenizeFile(filename, tokens, &lineIndex);

        for (int i = 0; i < lineIndex; ++i)
        {
            if (strcmp(target, tokens[i][0]) == 0)
            {
                char * message1 = "Found...\nName: ";
                char * message2 = "\nGrade: ";

                write(STDOUT_FILENO, message1, strlen(message1));
                write(STDOUT_FILENO, tokens[i][0], strlen(tokens[i][0]));
                write(STDOUT_FILENO, message2, strlen(message2));
                write(STDOUT_FILENO, tokens[i][1], strlen(tokens[i][1]));
                write(STDOUT_FILENO, "\n", 1);
                foundFlag = 1;
            }
        }

        if (foundFlag == 0)
        {
            char * message = "Cannot found ";

            write(STDOUT_FILENO, message, strlen(message));
            write(STDOUT_FILENO, target, strlen(target));
            write(STDOUT_FILENO, "\n", 1);
        }
        prepareLogFormat("searchStudent", target, filename, "");
        exit(EXIT_SUCCESS);
    }

    else
    {
        int status;

        waitpid(childPID, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            afterWaitErr("searchStudentCMD", WEXITSTATUS(status));
        }
    }
}

void sortAllCMD(const char * filename)
{
    pid_t childPID = fork();

    if (childPID == -1)
    {
        handleError("Fork error in the sortAllCMD.\n");
    }

    else if (childPID == 0)
    {
        int sortSelection, sortType, lineIndex = 0;
        char tokens[MAX_LINES][2][FBUFFER_SIZE];

        sortMenu(&sortSelection, &sortType);
        readAndTokenizeFile(filename, tokens, &lineIndex);

        if (sortSelection == 1)
        {
            if (sortType == 1)
            {
                for (int i = 0; i < lineIndex - 1; ++i)
                {
                    for (int j = 0; j < lineIndex - i - 1; ++j)
                    {
                        if (strcmp(tokens[j][0], tokens[j + 1][0]) > 0)
                        {
                            char firstIndexTemp[FBUFFER_SIZE];
                            char secondIndexTemp[FBUFFER_SIZE];

                            strcpy(firstIndexTemp, tokens[j][0]);
                            strcpy(secondIndexTemp, tokens[j][1]);
                            strcpy(tokens[j][0], tokens[j + 1][0]);
                            strcpy(tokens[j][1], tokens[j + 1][1]);
                            strcpy(tokens[j + 1][0], firstIndexTemp);
                            strcpy(tokens[j + 1][1], secondIndexTemp);
                        }
                    }
                }
            }

            else if (sortType == 2)
            {
                for (int i = 0; i < lineIndex - 1; ++i)
                {
                    for (int j = 0; j < lineIndex - i - 1; ++j)
                    {
                        if (strcmp(tokens[j][0], tokens[j + 1][0]) < 0)
                        {
                            char firstIndexTemp[FBUFFER_SIZE];
                            char secondIndexTemp[FBUFFER_SIZE];

                            strcpy(firstIndexTemp, tokens[j][0]);
                            strcpy(secondIndexTemp, tokens[j][1]);
                            strcpy(tokens[j][0], tokens[j + 1][0]);
                            strcpy(tokens[j][1], tokens[j + 1][1]);
                            strcpy(tokens[j + 1][0], firstIndexTemp);
                            strcpy(tokens[j + 1][1], secondIndexTemp);
                        }
                    }
                }
            }
        }

        else if (sortSelection == 2)
        {
            if (sortType == 1)
            {
                for (int i = 0; i < lineIndex - 1; ++i)
                {
                    for (int j = 0; j < lineIndex - i - 1; ++j)
                    {
                        if (strcmp(tokens[j][1], tokens[j + 1][1]) > 0)
                        {
                            char firstIndexTemp[FBUFFER_SIZE];
                            char secondIndexTemp[FBUFFER_SIZE];

                            strcpy(firstIndexTemp, tokens[j][0]);
                            strcpy(secondIndexTemp, tokens[j][1]);
                            strcpy(tokens[j][0], tokens[j + 1][0]);
                            strcpy(tokens[j][1], tokens[j + 1][1]);
                            strcpy(tokens[j + 1][0], firstIndexTemp);
                            strcpy(tokens[j + 1][1], secondIndexTemp);
                        }
                    }
                }
            }

            else if (sortType == 2)
            {
                for (int i = 0; i < lineIndex - 1; ++i)
                {
                    for (int j = 0; j < lineIndex - i - 1; ++j)
                    {
                        if (strcmp(tokens[j][1], tokens[j + 1][1]) < 0)
                        {
                            char firstIndexTemp[FBUFFER_SIZE];
                            char secondIndexTemp[FBUFFER_SIZE];

                            strcpy(firstIndexTemp, tokens[j][0]);
                            strcpy(secondIndexTemp, tokens[j][1]);
                            strcpy(tokens[j][0], tokens[j + 1][0]);
                            strcpy(tokens[j][1], tokens[j + 1][1]);
                            strcpy(tokens[j + 1][0], firstIndexTemp);
                            strcpy(tokens[j + 1][1], secondIndexTemp);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < lineIndex; ++i)
        {
            write(STDOUT_FILENO, tokens[i][0], strlen(tokens[i][0]));
            write(STDOUT_FILENO, " ", 1);
            write(STDOUT_FILENO, tokens[i][1], strlen(tokens[i][1]));
            write(STDOUT_FILENO, "\n", 1);
        }
        prepareLogFormat("sortAll", filename, "", "");
        exit(EXIT_SUCCESS);
    }

    else
    {
        int status;

        waitpid(childPID, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            afterWaitErr("sortAllCMD", WEXITSTATUS(status));
        }
    }
}

void showAllCMD(const char * filename)
{
    pid_t childPID = fork();

    if (childPID == -1)
    {
        handleError("Fork error in the showAllCMD.\n");
    }

    else if (childPID == 0)
    {
        char tokens[MAX_LINES][2][FBUFFER_SIZE];
        int lineIndex = 0;

        readAndTokenizeFile(filename, tokens, &lineIndex);

        for (int i = 0; i < lineIndex; ++i)
        {
            write(STDOUT_FILENO, tokens[i][0], strlen(tokens[i][0]));
            write(STDOUT_FILENO, " ", 1);
            write(STDOUT_FILENO, tokens[i][1], strlen(tokens[i][1]));
            write(STDOUT_FILENO, "\n", 1);
        }
        prepareLogFormat("showAll", filename, "", "");
        exit(EXIT_SUCCESS);
    }

    else
    {
        int status;

        waitpid(childPID, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            afterWaitErr("showAllCMD", WEXITSTATUS(status));
        }
    }
}

void listGradesCMD(const char * filename)
{
	pid_t childPID = fork();

    if (childPID == -1)
    {
        handleError("Fork error in the listGradesCMD.\n");
    }

    else if (childPID == 0)
	{
		char tokens[MAX_LINES][2][FBUFFER_SIZE];
        int lineIndex = 0;

        readAndTokenizeFile(filename, tokens, &lineIndex);

        for (int i = 0; i < 5 && i < lineIndex; ++i)
        {
        	if (tokens[i][0] != NULL && tokens[i][1] != NULL)
        	{
        		write(STDOUT_FILENO, tokens[i][0], strlen(tokens[i][0]));
	            write(STDOUT_FILENO, " ", 1);
	            write(STDOUT_FILENO, tokens[i][1], strlen(tokens[i][1]));
	            write(STDOUT_FILENO, "\n", 1);
        	}
        }
        prepareLogFormat("listGrades", filename, "", "");
        exit(EXIT_SUCCESS);
	}

	else
    {
        int status;

        waitpid(childPID, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            afterWaitErr("listGradesCMD", WEXITSTATUS(status));
        }
    }
}

void listSomeCMD(const char * filename, const int pageAmount, const int pageIndex)
{
	pid_t childPID = fork();

	if (childPID == -1)
    {
        handleError("Fork error in the listSomeCMD.\n");
    }

    else if (childPID == 0)
	{
		char tokens[MAX_LINES][2][FBUFFER_SIZE];
        int lineIndex = 0;

        int realIndexStart = (pageIndex - 1) * pageAmount;
        int realIndexFinish = realIndexStart + pageAmount;

        readAndTokenizeFile(filename, tokens, &lineIndex);

        if (realIndexStart >= lineIndex) 
        {
        	char * message = "Error: Page index is out of range in the listSome command.\n"; 
	        write(STDOUT_FILENO, message, strlen(message));
        }
		        
        for (int i = realIndexStart; i < realIndexFinish && i < lineIndex; ++i)
        {
        	if (tokens[i][0] != NULL && tokens[i][1] != NULL)
        	{
	            write(STDOUT_FILENO, tokens[i][0], strlen(tokens[i][0]));
	            write(STDOUT_FILENO, " ", 1);
	            write(STDOUT_FILENO, tokens[i][1], strlen(tokens[i][1]));
	            write(STDOUT_FILENO, "\n", 1);
	        }
        }
        char numStr1[5];
        char numStr2[5];

        snprintf(numStr1, sizeof(numStr1), "%d", pageAmount);
        snprintf(numStr2, sizeof(numStr2), "%d", pageIndex);

        prepareLogFormat("listSome", numStr1, numStr2, filename);
        exit(EXIT_SUCCESS);
	}

	else
    {
        int status;

        waitpid(childPID, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            afterWaitErr("listSomeCMD", WEXITSTATUS(status));
        }
    }	
}
