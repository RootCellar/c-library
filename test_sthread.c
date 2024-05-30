
#include <time.h>

#include "sthread.h"
#include "test.h"
#include "benchmark.h"
#include "debug.h"

#define NUM_LOOPS_SIMPLE 10000000

void increment_values(int id, int thread_count, int count, void* data) {
  debug_printf("%d", id);
  int* actual_data = (int*) data;
  for(int i = 0; i < NUM_LOOPS_SIMPLE; i++)
    actual_data[id]++;
}

void main() {

  srand(time(NULL));

  SECTION("sthread Simple");

  int thread_count = 12;
  int starting_value = 6;
  int data[thread_count];

  for(int i = 0; i < thread_count; i++) {
    data[i] = starting_value;
  }

  run_in_threads(increment_values, (void*) data, thread_count, thread_count);

  for(int i = 0; i < thread_count; i++) {
    debug_printf("%d", data[i]);
  }

  for(int i = 0; i < thread_count; i++) {
    debug_printf("%d", data[i]);
    TEST( data[i] == starting_value + NUM_LOOPS_SIMPLE, "spot was incremented" );
  }

  exit(0);

}