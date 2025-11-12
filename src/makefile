CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
LDFLAGS =

VALGRIND=valgrind -s --log-fd=1 --time-stamp=yes --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes --read-var-info=yes --tool=memcheck
LINTER=cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive --check-level=exhaustive --library=posix

LIB_SOURCE=$(wildcard lib/*.c)
LIB_HEADERS=$(wildcard lib/*.h)

ASSEMBLIES=$(LIB_SOURCE:lib/%.c=build/asm/%.asm)
OBJECTS=$(LIB_SOURCE:%.c=build/%.o)

EXEC_SOURCE=$(wildcard programs/*.c)
EXECUTABLES=$(EXEC_SOURCE:%.c=build/%)
TEST_PROGRAMS=$(wildcard build/programs/test_*)

all: executables
everything: assemblies objects executables

.PHONY: all everything assemblies objects executables clean check test valgrind lint
.SILENT: test

clean:
	rm -rf build

check: all test lint

lint:
	$(LINTER) .

assemblies: $(ASSEMBLIES)
objects: $(OBJECTS)
executables: $(EXECUTABLES)

build/lib: build
	mkdir -p build/lib

build/programs: build
	mkdir -p build/programs

build/asm: build
	mkdir -p build/asm

build:
	mkdir -p build

build/lib/%.o: lib/%.c build/lib
	$(CC) $(CFLAGS) -c $< -o $@

build/programs/%: programs/%.c $(OBJECTS) build/programs
	$(CC) $(CFLAGS) $< -o $@ $(OBJECTS) -lm -lpthread

build/asm/%.asm: lib/%.c build/asm
	$(CC) $(CFLAGS) -S -o $@ $<

run_%: build/programs/%
	./$<

test: executables
	for i in $(TEST_PROGRAMS); do \
  		echo; \
  		echo $$i; \
  		echo; \
		./$$i || exit 1; \
	done \

valgrind: executables
	for i in $(TEST_PROGRAMS); do \
  		echo; \
  		echo $$i; \
  		echo; \
		time $(VALGRIND) ./$$i || exit 1; \
	done \
