
#include <time.h>

#include "sthread.h"
#include "benchmark.h"
#include <math.h>

#define NUM_LOOPS_SIMPLE 100000
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

  int data[THREAD_COUNT];

  for(int i = 0; i < THREAD_COUNT; i++) {
    data[i] = 0;
  }

  TIMES( BENCHMARK_LOOPS( run_in_threads(modify_values, (void*) data, THREAD_COUNT, THREAD_COUNT), "", 100), 4 );

  exit(0);

}
