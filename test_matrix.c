#include <stdio.h>
#include <stdlib.h>

#include "test.h"
#include "benchmark.h"
#include "memory.h"
#include "matrix.h"

int main() {

  int wid = 3;
  int ht = 3;
  int dep = 3;
  int dim_4 = 3;

  int* matrix = tMalloc(sizeof(int) * wid * ht * dep * dim_4);

  size_t prev_idx = -1;
  size_t idx;

  for(int w = 0; w < dim_4; w++)
  for(int z = 0; z < dep; z++)
  for(int y = 0; y < ht; y++)
  for(int x = 0; x < wid; x++) {
    idx = index_4d(x, y, z, w, wid, ht, dep);
    debug_printf("idx: %zu (%d, %d, %d, %d)", idx, x, y, z, w);
    TEST( idx == prev_idx + 1, "index increments");
    prev_idx = idx;
  }

  SHOW_TEST_RESULTS();

  SECTION("Benchmarks");

  BENCHMARK_LOOPS(index_2d(1, 2, 3), "index_2d", 50000000);
  BENCHMARK_LOOPS(index_3d(1, 2, 3, 3, 3), "index_3d", 50000000);
  BENCHMARK_LOOPS(index_4d(1, 2, 3, 4, 3, 3, 3), "index_4d", 50000000);

  tFree(matrix);
  tFreePointerList();

  exit(0);
}
