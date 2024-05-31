.SILENT:
	
CC = gcc
CFLAGS = -Wall -pedantic-errors -std=gnu99
LIBS = -lm
DEPS = helper.h
OBJ = .o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: program run clean

program: main$(OBJ) helper$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: program
	./program

clean:
	rm -f *.o program
	