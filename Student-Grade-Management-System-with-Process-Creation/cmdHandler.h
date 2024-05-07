#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>	
#include <unistd.h>
#include <sys/wait.h>

void manGTUCMD();
void gtuStudentGradesCMD(const char *);
void addStudentGradeCMD(const char *, const char *, char *);
void searchStudentCMD(const char *, const char *);
void sortAllCMD(const char *);
void showAllCMD(const char *);
void listGradesCMD(const char *);
void listSomeCMD(const char *, const int, const int);

#endif /* CMD_HANDLER_H */
