#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "debug.h"
#include "benchmark.h"
#include "memory.h"
#include "random.h"

#define BENCHMARK_TIMES (3)

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

int recursive_fibonacci(int i) {
  if(i == 0 || i == 1) return 1;
  return recursive_fibonacci(i - 1) + recursive_fibonacci(i - 2);
}

void bubblesort(int* nums, size_t size) {

  int temp;

  for(size_t i = 0; i < size; i++) {
    for(size_t j = 0; j < size-1; j++) {
      if(nums[j+1] < nums[j]) {
        temp = nums[j+1];
        nums[j+1] = nums[j];
        nums[j] = temp;
      }
    }
  }
}

// float benchmarks

float float_add(float f) {
  volatile float result;
  result = f + 3.3f;
  return result;
}

float float_subtract(float f) {
  volatile float result;
  result = f - 3.3f;
  return result;
}

float float_multiply(float f) {
  volatile float result;
  result = f * 3.3f;
  return result;
}

float float_divide(float f) {
  volatile float result;
  result = f / 3.3f;
  return result;
}

float float_many(float f) {
  volatile float v = f + 5.5;
  v += 8.7;
  v *= 6.54;
  v *= 32.17;
  v -= 7.65897;
  v -= 1.67891;
  v /= 17.19;
  v /= 41.23423;

  return v;
}

float float_sine(float f) {
  volatile float result;
  result = sinf(f);
  return result;
}

float float_cosine(float f) {
  volatile float result;
  result = cosf(f);
  return result;
}

float float_tangent(float f) {
  volatile float result;
  result = tanf(f);
  return result;
}

// double benchmarks

double double_add(double f) {
  volatile double result;
  result = f + 3.3;
  return result;
}

double double_subtract(double f) {
  volatile double result;
  result = f - 3.3;
  return result;
}

double double_multiply(double f) {
  volatile double result;
  result = f * 3.3;
  return result;
}

double double_divide(double f) {
  volatile double result;
  result = f / 3.3;
  return result;

}

// int benchmarks

int int_add(int i) {
  volatile int result;
  result = i + 3;
  return result;
}

int int_subtract(int i) {
  volatile int result;
  result = i - 3;
  return result;
}

int int_multiply(int i) {
  volatile int result;
  result = i * 3;
  return result;
}

int int_divide(int i) {
  volatile int result;
  result = i / 3;
  return result;
}

int int_mod(int i) {
  volatile int result;
  result = i % 3;
  return result;
}

// long benchmarks

long long_add(long i) {
  volatile long result;
  result = i + 3;
  return result;
}

long long_subtract(long i) {
  volatile long result;
  result = i - 3;
  return result;
}

long long_multiply(long i) {
  volatile long result;
  result = i * 3;
  return result;
}

long long_divide(long i) {
  volatile long result;
  result = i / 3;
  return result;
}

long long_mod(long i) {
  volatile long result;
  result = i % 3;
  return result;
}

// logic operations

int left_shift(int i) {
  volatile int result;
  result = i << 3;
  return result;
}

int right_shift(int i) {
  volatile int result;
  result = i >> 3;
  return result;
}

int logic_and(int i) {
  volatile int result;
  result = i & 3;
  return result;
}

int logic_or(int i) {
  volatile int result;
  result = i | 3;
  return result;
}

// other

void empty_function() {

}

// benchmark helper functions

void test_bubblesort(size_t size) {
  int* nums = malloc(sizeof(int) * size);
  if(nums == NULL) return;

  for(size_t i = 0; i < size; i++) {
    nums[i] = size - i;
  }

  bubblesort(nums, size);
  free(nums);
}

int main() {
  srand(time(NULL));

  benchmark_setupLocale();

  // System benchmarks

  printf("\n\n System benchmarks  \n\n");

  // empty function

  TIMES( BENCHMARK_LOOPS( empty_function(), "empty_function", 10000000 ), BENCHMARK_TIMES);

  // rand

  TIMES( BENCHMARK_LOOPS( rand(), "rand", 1000000 ), BENCHMARK_TIMES);

  // clock

  TIMES( BENCHMARK_LOOPS( clock(), "clock", 100000 ), BENCHMARK_TIMES);

  // float benchmarks

  printf("\n Float benchmarks  \n\n");

  int float_times = 100000000;

  TIMES( BENCHMARK_LOOPS( float_add(9.0), "float_add", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_subtract(9.0), "float_subtract", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_multiply(9.0), "float_multiply", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_divide(9.0), "float_divide", float_times ), BENCHMARK_TIMES);

  TIMES( BENCHMARK_LOOPS( float_many(9.0), "float_many", float_times / 10 ), BENCHMARK_TIMES);

  TIMES( BENCHMARK_LOOPS( float_cosine(1.42), "float_cosine", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_sine(1.42), "float_sine", float_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( float_tangent(1.42), "float_tangent", float_times ), BENCHMARK_TIMES);

  // int benchmarks

  printf("\n Integer benchmarks  \n\n");

  int int_times = 100000000;

  TIMES( BENCHMARK_LOOPS( int_add(9), "int_add", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_subtract(9), "int_subtract", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_multiply(9), "int_multiply", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_divide(9), "int_divide", int_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( int_mod(9), "int_mod", int_times ), BENCHMARK_TIMES);

  // double benchmarks

  printf("\n Double benchmarks  \n\n");

  int double_times = 100000000;

  TIMES( BENCHMARK_LOOPS( double_add(9.0), "double_add", double_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( double_subtract(9.0), "double_subtract", double_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( double_multiply(9.0), "double_multiply", double_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( double_divide(9.0), "double_divide", double_times ), BENCHMARK_TIMES);

  // long benchmarks

  printf("\n Long benchmarks  \n\n");

  long long_times = 100000000;

  TIMES( BENCHMARK_LOOPS( long_add(9), "long_add", long_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( long_subtract(9), "long_subtract", long_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( long_multiply(9), "long_multiply", long_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( long_divide(9), "long_divide", long_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( long_mod(9), "long_mod", long_times ), BENCHMARK_TIMES);

  // logic benchmarks

  printf("\n Logic benchmarks  \n\n");

  int logic_times = 100000000;

  TIMES( BENCHMARK_LOOPS( left_shift(9), "left_shift", logic_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( right_shift(9), "right_shift", logic_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( logic_and(1180), "logic_and", logic_times ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( logic_or(1180), "logic_or", logic_times ), BENCHMARK_TIMES);

  // real world benchmarks

  printf("\n Real world benchmarks  \n\n");

  // Hit

  float modifier = 0.0;
  TIMES( BENCHMARK_LOOPS( hit(0.05,&modifier), "hit", 1000000 ), BENCHMARK_TIMES);

  // Dice Roll

  TIMES( BENCHMARK_LOOPS( roll_die(8), "roll_die", 100000), BENCHMARK_TIMES);

  // malloc_and_free

  TIMES( BENCHMARK_LOOPS( malloc_and_free(), "malloc_and_free", 1000000 ), BENCHMARK_TIMES);

  // recursive_fibonacci

  TIMES( BENCHMARK_LOOPS( recursive_fibonacci(3), "recursive_fibonacci(3)", 5000 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( recursive_fibonacci(5), "recursive_fibonacci(5)", 5000 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( recursive_fibonacci(10), "recursive_fibonacci(10)", 5000 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( recursive_fibonacci(15), "recursive_fibonacci(15)", 5000 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( recursive_fibonacci(20), "recursive_fibonacci(20)", 5000 ), BENCHMARK_TIMES);

  // test_bubblesort

  // test_bubblesort(10);
  TIMES( BENCHMARK_LOOPS( test_bubblesort(20), "test_bubblesort(20)", 100000 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( test_bubblesort(100), "test_bubblesort(100)", 1000 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( test_bubblesort(1000), "test_bubblesort(1,000)", 100 ), BENCHMARK_TIMES);
  TIMES( BENCHMARK_LOOPS( test_bubblesort(4000), "test_bubblesort(10,000)", 1 ), BENCHMARK_TIMES);

  tFreePointerList();
  exit(0);
}
