
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sthread.h"
#include "benchmark.h"
#include "vector.h"

#define BENCHMARK_LOOPS 100
#define NUM_LOOPS_TRIG 500000
#define NUM_LOOPS_VECTOR 50000000
#define THREAD_COUNT 16

void modify_values(int id, int thread_count, int count, void* data) {
  float* actual_data = (float*) data;
  for(int i = 0; i < NUM_LOOPS_TRIG; i++) {
    actual_data[id] += cosf(i);
    actual_data[id] += sinf(i);
  }
}

void vector_operations(int id, int thread_count, int count, void* data) {
  float* actual_data = (float*) data;
  struct Vector3 one = vector3_create(1,1,1);
  struct Vector3 two = vector3_create(2,2,2);
  for(int i = 0; i < NUM_LOOPS_VECTOR; i++) {
    one = vector3_cross_product(one, two);
    one = vector3_to_length(one, 7.0f);
    actual_data[id] += vector3_dot_product(one, two);
  }
}

void main() {

  srand(time(NULL));

  benchmark_setupLocale();

  int data[THREAD_COUNT];

  for(int i = 0; i < THREAD_COUNT; i++) {
    data[i] = 0;
  }


  float runs_per_second;

  for(int i = 0; i < 5; i++) {
    BENCHMARK_LOOPS_CODE( run_in_threads(modify_values, (void*) data, THREAD_COUNT, THREAD_COUNT), runs_per_second, BENCHMARK_LOOPS);

    printf("Total trig ops per second: %'f\n", runs_per_second * (float) NUM_LOOPS_TRIG * (float) THREAD_COUNT);
  }

  for(int i = 0; i < 5; i++) {
    BENCHMARK_LOOPS_CODE( run_in_threads(vector_operations, (void*) data, THREAD_COUNT, THREAD_COUNT), runs_per_second, BENCHMARK_LOOPS);

    printf("Total vector ops per second: %'f\n", runs_per_second * (float) NUM_LOOPS_VECTOR * (float) THREAD_COUNT);
  }

  exit(0);

}
