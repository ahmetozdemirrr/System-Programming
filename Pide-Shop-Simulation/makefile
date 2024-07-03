.SILENT:

CC = gcc

CFLAGS = -Wall -pedantic-errors -std=gnu99
CFLAGS += -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
CFLAGS += -D_FILE_OFFSET_BITS=64

LIBS = -lm

DEPS = systemHelper.h queue.h

OBJ = oven.o customer.o queue.o systemHelper.o

# general rules
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# targets
all: PideShop HungryVeryMuch

PideShop: oven.o systemHelper.o queue.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

HungryVeryMuch: customer.o systemHelper.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

######## memory check targets ########

valgrind-oven: PideShop
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./PideShop 127.0.0.1 4 6 1

valgrind-customer: HungryVeryMuch
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./HungryVeryMuch

######################################

######## thread check targets ########

helgrind-oven: PideShop
	valgrind --tool=helgrind ./PideShop

helgrind-customer: HungryVeryMuch
	valgrind --tool=helgrind ./HungryVeryMuch

######################################

# clean rule
clean:
	rm -f *.o PideShop HungryVeryMuch *.log

.PHONY: all clean valgrind-oven valgrind-customer helgrind-oven helgrind-customer
