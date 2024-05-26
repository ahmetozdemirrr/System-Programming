.SILENT:

CC = gcc
CFLAGS = -Wall -pedantic-errors -std=gnu99 -pthread
CFLAGS += -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
CFLAGS += -D_FILE_OFFSET_BITS=64
LIBS = -lm
DEPS = systemHelper.h stack.h
OBJ = systemHelper.o stack.o main.o 

# general rules
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# TARGETS:

all: MWCp

# main target
MWCp: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# mem-check targets
valgrind: MWCp
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./MWCp

# thread safe control
# helgrind: MWCp
#	valgrind --tool=helgrind ./MWCp

# clean rule
clean:
	rm -f *.o MWCp

.PHONY: all clean valgrind
