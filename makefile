CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS =

VALGRIND=valgrind -s --log-fd=1 --time-stamp=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --read-var-info=yes --tool=memcheck
LINTER=cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --check-level=exhaustive --library=posix

HEADERS = $(wildcard *.h)

.PHONY: all clean run_benchmark check test

all: benchmark benchmark_threads test_memory test_statistics test_math test_prime test_inet test_sthread

clean:
	rm -f benchmark benchmark_threads test_memory test_statistics test_math test_prime test_inet test_sthread

check: all test lint

lint:
	 $(LINTER) *.c

test: all
	time $(VALGRIND) ./test_memory
	time $(VALGRIND) ./test_statistics
	time $(VALGRIND) ./test_math
	time $(VALGRIND) ./test_inet
	time $(VALGRIND) ./test_sthread

run_benchmark: all
	./benchmark
	./benchmark_threads

benchmark: benchmark.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ benchmark.c

benchmark_threads: benchmark_threads.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ benchmark_threads.c -lpthread -lm

test_memory: test.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test.c

test_statistics: test_statistics.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_statistics.c -lm

test_math: test_math.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_math.c -lm

test_prime: prime.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ prime.c -lm

test_inet: test_inet.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_inet.c

test_sthread: test_sthread.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_sthread.c -lpthread
