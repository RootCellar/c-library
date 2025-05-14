#include "unit_testing.h"
#include "memory.h"

int main() {

  SECTION("Basic checks");

  TEST( tResize(-1) == 1, "tResize(-1)");
  TEST( tResize(0) == 1, "tResize(0)");

  TEST( tResize(1) == 0, "tResize(1)");
  TEST( tResize(1) == 0, "tResize(1) again");

  TEST( tMalloc(0) == NULL, "tMalloc(0)");
  TEST( tRealloc(NULL, 16) == NULL, "tRealloc(NULL, 16)");

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() with nothing allocated");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() with nothing allocated");



  SECTION("Using the memory allocator");



  size_t alloc_size = 10;
  size_t alloc_bytes = alloc_size * sizeof(int);

  int* ptr = tMalloc(alloc_bytes);
  TEST( ptr != NULL, "tMalloc() a pointer");
  TEST( tFreePointerList() == 1, "Attempt to free pointer list while item is allocated");
  TEST( tGetTotalAllocs() == 1, "tGetTotalAllocs() == 1");
  TEST( tGetTotalAllocSize() == alloc_bytes, "tGetTotalAllocSize()");

  for(size_t i = 0; i < alloc_size; i++) {
    TEST( tOwnsAddress(&ptr[i]) == 1, "tOwnsAddress() in bounds");
  }
  
  TEST( tOwnsAddress(&ptr[alloc_size + 1]) == 0, "tOwnsAddress() past bound");
  TEST( tOwnsAddress(ptr - 1) == 0, "tOwnsAddress() before bound");

  tFree(ptr);
  TEST( tHasPtr(ptr) == 0, "Pointer list doesn't have pointer that was just freed");

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() after tFree()");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() after tFree()");



  SECTION("Pointer list size is exactly 1");



  ptr = tMalloc(alloc_bytes);
  int* ptr2;
  TEST( ptr != NULL, "Successfully allocate memory");
  TEST( tMalloc(alloc_bytes) == NULL, "pointer list too small");

  ptr2 = ptr;
  ptr = tRealloc(ptr, alloc_bytes + 16);
  TEST( ptr != NULL, "can reallocate the chunk of memory");
  TEST( tOwnsAddress(ptr) == 1, "tOwnsAddress() on reallocated memory chunk");
  TEST( tOwnsAddress(ptr2) == 0, "tOwnsAddress() on free'd memory chunk");

  TEST( (ptr = tRealloc(ptr, 1)) != NULL, "realloc pointer to smaller chunk of memory");

  TEST( tResize(10) == 0, "expand pointer list with one chunk of memory allocated");
  TEST( tResize(1) == 0, "shrink pointer list to fit the one pointer");

  tFree(ptr);
  TEST( tHasPtr(ptr) == 0, "Pointer list doesn't have pointer that was just freed");

  TEST( tResize(10) == 0, "expand pointer list");
  TEST( tResize(1) == 0, "shrink pointer list");

  ptr = tMalloc(alloc_bytes);
  TEST( tMalloc(alloc_bytes) == NULL, "tMalloc() pointer list too small");
  TEST( tOwnsAddress(ptr) == 1, "tOwnsAddress() on pointer");
  tFree(ptr);
  TEST( tHasPtr(ptr) == 0, "Pointer list doesn't have pointer that was just freed");

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() after tFree()");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() after tFree()");
  TEST( tOwnsAddress(ptr) == 0, "tOwnsAddress() on pointer after free");



  SECTION("Pointer List expanded to 10 items");



  TEST( tResize(10) == 0, "expand pointer list");



  SECTION("Allocate and free many pointers");



  size_t ptr_array_size = 10;
  int* ptr_array[ptr_array_size];

  TEST( tResize(10) == 0, "expand pointer list");
  for(size_t i = 0; i < ptr_array_size; i++) {
    ptr_array[i] = tMalloc(alloc_bytes);
    TEST( ptr_array[i] != NULL, "can allocate multiple pointers");
    TEST( tHasPtr(ptr_array[i]), "Pointer list has pointer that was just allocated");
  }
  
  TEST( tMalloc(alloc_bytes) == NULL, "can't allocate another pointer");
  TEST( tGetTotalAllocs() == 10, "tGetTotalAllocs() is correct");  
  TEST( tGetTotalAllocSize() == ptr_array_size * alloc_bytes, "tGetTotalAllocSize() is correct");

  for(size_t i = 0; i < ptr_array_size; i++) {
    tFree(ptr_array[i]);
    TEST( tHasPtr(ptr_array[i]) == 0, "Pointer list doesn't have pointer that was just freed");
  }

  TEST( tGetTotalAllocs() == 0, "Pointer list is empty at end of test");
  TEST( tGetTotalAllocSize() == 0, "Pointer list is empty at end of test");



  SECTION("Allocate pointers, free some, shrink pointer list");



  for(size_t i = 0; i < ptr_array_size; i++) { ptr_array[i] = 0; }

  for(size_t i = 0; i < ptr_array_size; i++) {
    ptr_array[i] = tMalloc(alloc_bytes);
    TEST( ptr_array[i] != NULL, "can allocate multiple pointers");
    TEST( tHasPtr(ptr_array[i]), "Pointer list has pointer that was just allocated");
  }

  int freed = 0;
  int spread = 2;
  
  for(size_t i = 0; i < ptr_array_size; i += spread) {
    tFree(ptr_array[i]);
    TEST( tHasPtr(ptr_array[i]) == 0, "Pointer list doesn't have pointer that was just freed");

    ++freed;
  }

  TEST( tResize(ptr_array_size - freed) == 0, "shrink pointer list");

  TEST( tGetTotalAllocs() == ptr_array_size - freed, "Pointer list has correct number of elements");
  TEST( tGetTotalAllocSize() == (ptr_array_size - freed) * alloc_bytes, "Pointer list has correct size");

  for(size_t i = 0; i < ptr_array_size; i++) {
    if(i % spread == 0) continue;
    tFree(ptr_array[i]);
    TEST( tHasPtr(ptr_array[i]) == 0, "Pointer list doesn't have pointer that was just freed");
  }



  SECTION("Pointer list is empty at end of test");

  

  TEST( tGetTotalAllocs() == 0, "Pointer list is empty at end of test");
  TEST( tGetTotalAllocSize() == 0, "Pointer list is empty at end of test");

  TEST( tFreePointerList() == 0, "Pointer list is successfully freed");

  SHOW_TEST_RESULTS();

  return 0;
  
}
