#include "test.h"
#include "memory.h"

void main() {
  TEST( tResize(-1) == 1, "tResize(-1)");
  TEST( tResize(0) == 1, "tResize(0)");

  TEST( tResize(1) == 0, "tResize(1)");
  TEST( tResize(1) == 0, "tResize(1) again");

  TEST( tMalloc(0) == NULL, "tMalloc(0)");

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() with nothing allocated");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() with nothing allocated");

  size_t alloc_size = 10;
  size_t alloc_bytes = alloc_size * sizeof(int);

  int* ptr = tMalloc(alloc_bytes);
  TEST( ptr != NULL, "tMalloc() a pointer");
  TEST( tGetTotalAllocs() == 1, "tGetTotalAllocs() == 1");
  TEST( tGetTotalAllocSize() == alloc_bytes, "tGetTotalAllocSize()");

  for(size_t i = 0; i < alloc_size; i++) {
    TEST( tOwnsAddress(&ptr[i]) == 1, "tOwnsAddress() in bounds");
  }
  
  TEST( tOwnsAddress(&ptr[alloc_size + 1]) == 0, "tOwnsAddress() past bound");
  TEST( tOwnsAddress(ptr - 1) == 0, "tOwnsAddress() before bound");

  TEST( tFree(ptr) == 0, "tFree() the pointer");

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() after tFree()");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() after tFree()");
  
}
