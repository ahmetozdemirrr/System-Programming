.SILENT:
	
CC = gcc
CFLAGS = -Wall -pedantic-errors -std=gnu99
LIBS = -lm
DEPS = cmdProcess.h cmdHandler.h
OBJ = .o
LOG_FILE = system.log

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: program run clean

program: main$(OBJ) cmdProcess$(OBJ) cmdHandler$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: program
	./program

clean:
	rm -f *.o *.txt program $(LOG_FILE)
	
