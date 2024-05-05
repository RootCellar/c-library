#include "test.h"
#include "memory.h"

void main() {

  // Basic checks

  TEST( tResize(-1) == 1, "tResize(-1)");
  TEST( tResize(0) == 1, "tResize(0)");

  TEST( tResize(1) == 0, "tResize(1)");
  TEST( tResize(1) == 0, "tResize(1) again");

  TEST( tFree(0) == 1, "free a null pointer");
  TEST( tFree( (void*) 16 ) == 1, "free a pointer that isn't in the list");

  TEST( tMalloc(0) == NULL, "tMalloc(0)");

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() with nothing allocated");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() with nothing allocated");

  // Using the memory allocator

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

  // Pointer list size is exactly 1

  ptr = tMalloc(alloc_bytes);
  TEST( tMalloc(alloc_bytes) == NULL, "pointer list too small");
  TEST( tFree(ptr) == 0, "tFree() the pointer");

  TEST( tResize(10) == 0, "expand pointer list");
  TEST( tResize(1) == 0, "shrink pointer list");

  ptr = tMalloc(alloc_bytes);
  TEST( tMalloc(alloc_bytes) == NULL, "tMalloc() pointer list too small");
  TEST( tOwnsAddress(ptr) == 1, "tOwnsAddress() on pointer");
  TEST( tFree(ptr) == 0, "tFree() the pointer");
  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() after tFree()");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() after tFree()");
  TEST( tOwnsAddress(ptr) == 0, "tOwnsAddress() on pointer after free");
  TEST( tFree(ptr) == 1, "tFree() the pointer again");

  TEST( tFindSpot(ptr) == -1, "find a pointer that is not in the list");

  // Pointer List expanded to 10 items

  size_t ptr_array_size = 10;
  int* ptr_array[ptr_array_size];
  
  TEST( tResize(10) == 0, "expand pointer list");
  for(size_t i = 0; i < ptr_array_size; i++) {
    ptr_array[i] = tMalloc(alloc_bytes);
    TEST( ptr_array[i] != NULL, "can allocate multiple pointers");
  }
  
  TEST( tMalloc(alloc_bytes) == NULL, "can't allocate another pointer");
  TEST( tGetTotalAllocs() == 10, "tGetTotalAllocs() is correct");  
  TEST( tGetTotalAllocSize() == ptr_array_size * alloc_bytes, "tGetTotalAllocSize() is correct");

  for(int i = 0; i < ptr_array_size; i++) {
    TEST( tFree(ptr_array[i]) == 0, "can free multiple pointers");
  }

  // Pointer list is empty at end of test

  TEST( tGetTotalAllocs() == 0, "Pointer list is empty at end of test");
  TEST( tGetTotalAllocSize() == 0, "Pointer list is empty at end of test");
}
