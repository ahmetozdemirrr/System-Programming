#include "cmdHandler.h"
#include "cmdProcess.h"

int main(int argc, char const *argv[]) 
{
    logToFile("The program start\n");

    const char * message = "Enter (q) for quit.\n";
	const char * quit = "q\n";

    char inputBuffer[INPUT_BUFFER];

    write(STDOUT_FILENO, message, strlen(message));

    while (TRUE) 
    {
        ssize_t bytesRead = read(STDIN_FILENO, inputBuffer, INPUT_BUFFER); /* Standart girişten okuma */

        if (bytesRead == -1)
        {
	        handleError("Error: read error - Command line (in main function)\n");
        }

        if (bytesRead == 0)
        {
        	break;
        }
        inputBuffer[bytesRead] = '\0'; /* Okunan veriyi NULL karakterle sonlandır */		
        
        if (strcmp(inputBuffer, quit) == 0)
        {
        	logToFile("Program terminated with input : q\n");
        	exit(EXIT_SUCCESS);
        }

        /************* Tokenize inputBuffer *************/

        char word[MAX_WORD_SIZE] = "";
        char * tokens[MAX_TOKENS];
        int tokenCount = 0;
        int inQuote = 0;

        for (int i = 0; i < strlen(inputBuffer); ++i) /* -1 for file name */
        {
            if (inputBuffer[i] == '\"') 
            {
                if (inQuote == 0) 
                {
                    inQuote = 1;
                    strcpy(word, "\""); /* Tırnak işaretiyle başla */
                } 

                else 
                {
                    inQuote = 0;
                    strcat(word, "\""); /* Tırnak işaretiyle bitir */
                    tokens[tokenCount++] = strdup(word);
                    strcpy(word, "");
                }
            } 

            else if ((inputBuffer[i] == ' ' && !inQuote) || i == strlen(inputBuffer) - 1) 
            {
                if (strlen(word) > 0) 
                {
                    tokens[tokenCount++] = strdup(word);
                    strcpy(word, "");
                }
            } 

            else 
            {
                strncat(word, &inputBuffer[i], 1);
            }
        }

    	/**************************************************/

        if (tokenCount > 1) /* tek parametreli yanlış girdilerde ve tek parametreli komutlarda segfault yememek için */
        {
        	trimQuotes(tokens[tokenCount - 1]); /* Dosya adının başındaki ve sonundaki tırnakları kaldır */

            if (endsWithTXT(tokens[tokenCount - 1]))
            {
                process(tokenCount, tokens, MAX_TOKENS);
            }

            else
            {
                fprintf(stderr, "Invalid file extension, please enter only .txt extension!\n");
            }
        }

        else if (tokenCount == 1)
        {
            process(tokenCount, tokens, MAX_TOKENS);
        }

        else
        {
            fprintf(stderr, "Enter a command!\n");
        }
        
        for (int i = 0; i < tokenCount; ++i) 
        {
            free(tokens[i]);
        }
    }
    logToFile("Program terminated.\n");

    return EXIT_SUCCESS;
}
