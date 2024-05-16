.SILENT:

CC = gcc
CFLAGS = -Wall -pedantic-errors -std=gnu99 -pthread
CFLAGS += -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
CFLAGS += -D_FILE_OFFSET_BITS=64
LIBS = -lm
DEPS = systemHelper.h
OBJ = systemHelper.o system.o

# general rules
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# TARGETS:

all: system

# main target
system: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# mem-check targets
valgrind: system
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./system

# thread safe control
helgrind: system
	valgrind --tool=helgrind ./system

# clean rule
clean:
	rm -f *.o system

.PHONY: all clean valgrind helgrind
