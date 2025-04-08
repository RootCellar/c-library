CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
LDFLAGS =

VALGRIND=valgrind -s --log-fd=1 --time-stamp=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --read-var-info=yes --tool=memcheck
LINTER=cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --check-level=exhaustive --library=posix

SOURCE=$(wildcard *.c)
HEADERS=$(wildcard *.h)

ASSEMBLIES=$(SOURCE:%.c=%.asm)
OBJECTS=$(SOURCE:%.c=%.o)

EXECUTABLES=benchmark benchmark_threads test_memory test_statistics test_math test_matrix test_prime test_inet test_sthread test_strings neural dice

all: executables

.PHONY: all assemblies objects executables clean run_benchmark check test

clean:
	rm -f $(ASSEMBLIES)
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLES)

check: all test lint

lint:
	$(LINTER) .

assemblies: $(ASSEMBLIES)
objects: $(OBJECTS)
executables: $(EXECUTABLES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.asm: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

test: all
	time $(VALGRIND) ./test_memory
	time $(VALGRIND) ./test_statistics
	time $(VALGRIND) ./test_math
	time $(VALGRIND) ./test_matrix
	time $(VALGRIND) ./test_inet
	time $(VALGRIND) ./test_sthread
	time $(VALGRIND) ./test_strings

run_benchmark: all
	./benchmark
	./benchmark_threads

benchmark: benchmark.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ benchmark.c -lm

benchmark_threads: benchmark_threads.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ benchmark_threads.c -lpthread -lm

test_memory: test.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test.c

test_statistics: test_statistics.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_statistics.c -lm

test_math: test_math.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_math.c -lm

test_matrix: test_matrix.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_matrix.c -lm

test_prime: prime.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ prime.c -lm

test_inet: test_inet.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_inet.c

test_sthread: test_sthread.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_sthread.c -lpthread

test_strings: test_strings.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ test_strings.c

neural: neural.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ neural.c -lm -lpthread

dice: dice.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ dice.c
