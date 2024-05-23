CC = gcc
CFLAGS = -g -Wall -Wextra 
LDFLAGS =

VALGRIND=valgrind -s --log-fd=1 --time-stamp=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --read-var-info=yes --tool=memcheck
LINTER=cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --check-level=exhaustive --library=posix 

HEADERS = $(wildcard *.h)

.PHONY: all clean run_benchmark check test

all: benchmark test_memory test_statistics test_prime test_inet

clean:
	rm benchmark test_memory test_statistics test_prime test_inet

check: all test lint

lint:
	 $(LINTER) *.c

test: all
	time $(VALGRIND) ./test_memory
	time $(VALGRIND) ./test_statistics
	time $(VALGRIND) ./test_inet

run_benchmark: all
	./benchmark

benchmark: benchmark.c $(HEADERS)
	$(CC) $(CFLAGS)-o $@ benchmark.c

test_memory: test.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test.c

test_statistics: test_statistics.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_statistics.c -lm

test_prime: prime.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ prime.c -lm

test_inet: test_inet.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_inet.c
