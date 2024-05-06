.SILENT:

CC = gcc
CFLAGS = -Wall -pedantic-errors -std=gnu99
CFLAGS += -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
CFLAGS += -D_FILE_OFFSET_BITS=64
LIBS = -lm
DEPS = systemHelper.h queue.h
OBJ = neHosServer.o neHosClient.o systemHelper.o queue.o

# Genel kurallar
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Hedefler
all: neHosServer neHosClient

neHosServer: neHosServer.o systemHelper.o queue.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

neHosClient: neHosClient.o systemHelper.o queue.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Hafıza kontrolü hedefleri
memcheck-server: neHosServer
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./neHosServer

memcheck-client: neHosClient
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./neHosClient

# Temizleme kuralı
clean:
	rm -f *.o neHosServer neHosClient *.log

.PHONY: all clean memcheck-server memcheck-client
