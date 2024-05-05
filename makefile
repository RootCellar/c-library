CC = gcc
CXXFLAGS = -g -Wall
LDFLAGS =

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp) $(wildcard *.h)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

.PHONY: all clean

all: benchmark test

clean:
	$(RM) benchmark test

benchmark: benchmark.c $(HEADERS)
	$(CC) -o $@ benchmark.c

test: test.c $(HEADERS)
	$(CC) -o $@ test.c
