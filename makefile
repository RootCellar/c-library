CXX = gcc
CXXFLAGS = -g -Wall
LDFLAGS =

SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.hpp) $(wildcard *.h)
OBJECTS = $(patsubst %.cpp,%.o,$(SOURCES))

.PHONY: all clean

all: benchmark

clean:
	$(RM) benchmark

benchmark: benchmark.c $(HEADERS)
	$(CXX) -o $@ benchmark.c
