
#include <time.h>

#include "../lib/sthread.h"
#include "../lib/unit_testing.h"
#include "../lib/benchmark.h"
#include "../lib/debug.h"

#define NUM_LOOPS_SIMPLE 10000000

void increment_values(size_t id, size_t thread_count, size_t count, void* data) {
  debug_printf("%zu", id);
  int* actual_data = (int*) data;
  for(int i = 0; i < NUM_LOOPS_SIMPLE; i++)
    actual_data[id]++;
}

void main() {

  srand(time(NULL));

  SECTION("sthread Simple");

  size_t thread_count = 12;
  int starting_value = 6;
  int data[thread_count];

  for(size_t i = 0; i < thread_count; i++) {
    data[i] = starting_value;
  }

  run_in_threads(increment_values, (void*) data, thread_count, thread_count);

  for(size_t i = 0; i < thread_count; i++) {
    debug_printf("%d", data[i]);
  }

  for(size_t i = 0; i < thread_count; i++) {
    debug_printf("%d", data[i]);
    TEST( data[i] == starting_value + NUM_LOOPS_SIMPLE, "spot was incremented" );
  }

  SHOW_TEST_RESULTS();

  exit(0);
}
