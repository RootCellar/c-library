
#include <time.h>
#include <math.h>
#include <stdio.h>

#include "sthread.h"
#include "benchmark.h"

#define NUM_LOOPS_SIMPLE 500000
#define BENCHMARK_LOOPS 100
#define THREAD_COUNT 16

void modify_values(int id, int count, void* data) {
  float* actual_data = (float*) data;
  for(int i = 0; i < NUM_LOOPS_SIMPLE; i++) {
    actual_data[id] += cosf(i);
    actual_data[id] += sinf(i);
  }
}

void main() {

  srand(time(NULL));

  benchmark_setupLocale();

  int data[THREAD_COUNT];

  for(int i = 0; i < THREAD_COUNT; i++) {
    data[i] = 0;
  }


  float speed;

  for(int i = 0; i < 5; i++) {
    BENCHMARK_LOOPS_CODE( run_in_threads(modify_values, (void*) data, THREAD_COUNT, THREAD_COUNT), speed, BENCHMARK_LOOPS);

    printf("Total trig ops per second: %'f\n", speed * (float) BENCHMARK_LOOPS * (float) NUM_LOOPS_SIMPLE * (float) THREAD_COUNT);
  }

  exit(0);

}
