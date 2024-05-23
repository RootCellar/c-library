CC = gcc
CFLAGS = -g -Wall -Wextra 
LDFLAGS =


.PHONY: all clean

all: benchmark test test_statistics test_prime test_inet

clean:
	rm benchmark test_memory test_statistics test_prime test_inet

benchmark: benchmark.c $(HEADERS)
	$(CC) $(CFLAGS)-o $@ benchmark.c

test: test.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test.c

test_statistics: test_statistics.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_statistics.c -lm

test_prime: prime.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ prime.c -lm

test_inet: test_inet.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_inet.c
