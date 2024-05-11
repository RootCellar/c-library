CC = gcc
CXXFLAGS = -g -Wall
LDFLAGS =

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp) $(wildcard *.h)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

.PHONY: all clean

all: benchmark test test_statistics test_prime test_inet

clean:
	$(RM) benchmark test test_statistics test_prime test_inet

benchmark: benchmark.c $(HEADERS)
	$(CC) -o $@ benchmark.c

test: test.c $(HEADERS)
	$(CC) -o $@ test.c

test_statistics: test_statistics.c $(HEADERS)
	$(CC) -o $@ test_statistics.c -lm

test_prime: prime.c $(HEADERS)
	$(CC) -o $@ prime.c -lm

test_inet: test_inet.c $(HEADERS)
	$(CC) -o $@ test_inet.c
