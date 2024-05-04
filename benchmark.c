#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "benchmark.h"
#include "memory.h"

#define BENCHMARK_TIMES (2)

// Real world workload type functions

int hit(float baseCrit, float* modifier) {
  float critChance = baseCrit + (*modifier);
  
  int roll = rand() % 1000;
  int rollValue = (int) (critChance * 1000);
  
  if(roll <= rollValue) {
    (*modifier) = 0.0;
    return 1;
  }
  (*modifier) += 0.03;
  return 0;
}

void malloc_and_free() {
  void* ptr = malloc(sizeof(char) * 1 << 8);
  free(ptr);
}

// float benchmarks

float float_add(float f) {
  return f + 5.5;
}

float float_subtract(float f) {
  return f - 5.5;
}

float float_multiply(float f) {
  return f * 5.5;
}

float float_divide(float f) {
  return f / 5.5;
}

// int benchmarks

int int_add(int i) {
  return i + 7;
}

int int_subtract(int i) {
  return i - 7;
}

int int_multiply(int i) {
  return i * 7;
}

int int_divide(int i) {
  return i / 7;
}

int int_mod(int i) {
  return i % 7;
}

int main() {
  srand(time(NULL));

  benchmark_setupLocale();

  // rand

  TIMES( BENCHMARK_LOOPS( rand(), "rand", 1000000 ), BENCHMARK_TIMES);

  // clock

  TIMES( BENCHMARK_LOOPS( clock(), "clock", 1000000 ), BENCHMARK_TIMES);

  // float benchmarks

  int float_times = 100000000;

  TIMES( BENCHMARK_LOOPS( float_add(9.0), "float_add", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_subtract(9.0), "float_subtract", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_multiply(9.0), "float_multiply", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_divide(9.0), "float_divide", float_times ), BENCHMARK_TIMES);

  // int benchmarks

  int int_times = 100000000;

  TIMES( BENCHMARK_LOOPS( int_add(9), "int_add", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_subtract(9), "int_subtract", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_multiply(9), "int_multiply", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_divide(9), "int_divide", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_mod(9), "int_mod", int_times ), BENCHMARK_TIMES);

  // Hit

  float modifier = 0.0;
  TIMES( BENCHMARK_LOOPS( hit(0.05,&modifier), "hit", 1000000 ), BENCHMARK_TIMES);

  // malloc_and_free

  TIMES( BENCHMARK_LOOPS( malloc_and_free(), "malloc_and_free", 1000000 ), BENCHMARK_TIMES);
  
}
