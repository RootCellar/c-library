/*
 *
 * Darian Marvel / RootCellar
 * 1/11/2023
 *
 * Memory allocator that keeps track of allocated pointers in a list.
 * This allows allocated bytes and pointers to be kept track of,
 * and for pointers to never really be lost.
 *
 * Supports manual list resizing - does not resize automatically.
 *
*/

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <string.h>

#include "debug.h"

#define MEMORY_DEBUG 0
#define memory_debug_printf(fmt, ...) if(MEMORY_DEBUG) debug_printf(fmt, __VA_ARGS__);
#define memory_debug_print(fmt) if(MEMORY_DEBUG) debug_print(fmt);

#define DEFAULT_POINTER_LIST_SIZE 1024

struct ptr_data {
  char* ptr;
  size_t size;
};

struct ptr_data* POINTER_LIST = NULL;
long int POINTER_LIST_SIZE = 0;

int is_valid_ptr(void* ptr) {
  return ptr != NULL;
}

// Determines whether or not the given address is owned
// based on what's in the pointer list
int tOwnsAddress(char* adr) {
  if(!is_valid_ptr(POINTER_LIST)) return 0;

  struct ptr_data* current;
  for(long int i = 0; i < POINTER_LIST_SIZE; i++) {
    if(is_valid_ptr(POINTER_LIST[i].ptr)) {
      current = &POINTER_LIST[i];
      if((uintptr_t) current->ptr <= (uintptr_t) adr
        && (uintptr_t) adr < (uintptr_t) (current->ptr + current->size)) {
        return 1;
      }
    }
  }

  return 0;
}

// Returns the number of allocations.
// Could be used to see if a function leaks memory on its own
long int tGetTotalAllocs() {
  if(!is_valid_ptr(POINTER_LIST)) return 0;

  long int count = 0;
  memory_debug_print("Counting all pointers...");

  for(long int i = 0; i < POINTER_LIST_SIZE; i++) {
    if(is_valid_ptr(POINTER_LIST[i].ptr)) {
      count += 1;
    }
  }

  return count;
}

unsigned long int tGetTotalAllocSize() {
  if(!is_valid_ptr(POINTER_LIST)) return 0;

  unsigned long int sum = 0;
  memory_debug_print("Finding total size of all pointers...");

  for(long int i = 0; i < POINTER_LIST_SIZE; i++) {
    if(is_valid_ptr(POINTER_LIST[i].ptr)) {
      sum += POINTER_LIST[i].size;
    }
  }

  return sum;
}

void tPrintStatus() {
  memory_debug_printf("There are %lu bytes allocated, amongst %ld pointers.", tGetTotalAllocSize(), tGetTotalAllocs());
  memory_debug_printf("The pointer list is %lu bytes, to hold %ld pointers.",
                      POINTER_LIST_SIZE * sizeof(struct ptr_data), POINTER_LIST_SIZE);
}

int tFreePointerList() {
  if(tGetTotalAllocs() > 0) {
    memory_debug_print("Not freeing pointer list: Items are still allocated!");
    return 1;
  }

  if(!is_valid_ptr(POINTER_LIST)) {
    memory_debug_print("Not freeing pointer list: List hasn't been allocated!");
    return 1;
  }

  free(POINTER_LIST);
  POINTER_LIST = NULL;
  POINTER_LIST_SIZE = 0;

  memory_debug_print("Freed pointer list");

  return 0;
}

// Find the given pointer.
// Handy trick: tFindSpot(0) can be used to find an empty slot
// to track a new pointer
long int tFindSpot(void* ptr) {
  if(!is_valid_ptr(POINTER_LIST)) return -1;

  memory_debug_printf("Finding pointer %p", ptr);
  for(long int i = 0; i < POINTER_LIST_SIZE; i++) {
    if(POINTER_LIST[i].ptr == ptr) {
      memory_debug_printf("Found pointer in slot %ld", i);
      return i;
    }
  }

  return -1;
}

// Get the size of the given pointer.
// Must be kept tracked of in the list.
// Returns -1 if the pointer is not found.
size_t tGetSize(void* ptr) {
  long int spot = tFindSpot(ptr);
  if(spot < 0) { return -1; }

  return POINTER_LIST[spot].size;
}

/* Condenses the list by defragmenting it.

   No return value.
*/
void tCondense() {
  memory_debug_print("Condensing list...");

  long int new_spot = tFindSpot(0);
  if(new_spot < 0) return;

  long int i = 0;
  while(is_valid_ptr(POINTER_LIST[i].ptr)) i++;

  for(; i < POINTER_LIST_SIZE; i++) {
    if(is_valid_ptr(POINTER_LIST[i].ptr)) {
      POINTER_LIST[new_spot].ptr = POINTER_LIST[i].ptr;
      POINTER_LIST[new_spot].size = POINTER_LIST[i].size;

      POINTER_LIST[i].ptr = NULL;
      POINTER_LIST[i].size = 0;

      memory_debug_printf(" %ld -> %ld ", i, new_spot);
      new_spot = tFindSpot(0);
    }
  }
}

/* Resize the pointer list to hold "len" items.
   Can increase OR decrease the size.
   Can also create a new pointer list if one does not exist

   Returns 0 if successful, 1 if there was a failure
*/
int tResize(long int len) {
  if(len <= 0) {
    memory_debug_print("Refusing size smaller than one...");
    return 1;
  }

  long int allocs = tGetTotalAllocs();
  memory_debug_printf("Attempting to resize pointer list... %ld -> %ld", POINTER_LIST_SIZE, len);
  if(len < allocs) {
    memory_debug_print("Refusing to allocate pointer list to be smaller than current number of allocations");
    return 1;
  }

  if(len == POINTER_LIST_SIZE) {
    memory_debug_print("Sizes equal, doing nothing...");
    return 0;
  }

  void* new_pointer_list = malloc(len * sizeof(struct ptr_data));
  if(!is_valid_ptr(new_pointer_list)) {
    memory_debug_print("Could not allocate new pointer list!");
    return 1;
  }

  memset(new_pointer_list, 0, len * sizeof(struct ptr_data));

  if(is_valid_ptr(POINTER_LIST)) {
    memory_debug_print("Replacing old pointer list...");
    tCondense();
    long int copy_length = len < POINTER_LIST_SIZE ? len : POINTER_LIST_SIZE;
    memcpy(new_pointer_list, POINTER_LIST, copy_length * sizeof(struct ptr_data));
    free(POINTER_LIST);
  }

  memory_debug_print("Success");

  POINTER_LIST_SIZE = len;
  POINTER_LIST = new_pointer_list;

  tPrintStatus();
  return 0;
}

// Add the given pointer, with len bytes allocated,
// to the list of tracked pointers
int tAdd(void* ptr, unsigned long int len) {
  if(!is_valid_ptr(POINTER_LIST)) {
    int failed = tResize(DEFAULT_POINTER_LIST_SIZE);
    if(failed) {
      // Could not create pointer list
      EXIT_FAIL();
    }
  }

  long int spot = tFindSpot(0);
  if(spot < 0) {
    return 1;
  }

  struct ptr_data* ptrData = &POINTER_LIST[spot];
  ptrData->ptr = ptr;
  ptrData->size = len;

  return 0;
}

// Allocate memory for a pointer and return the pointer,
// adding it to the tracked list if the memory allocation was successful
void* tMalloc(unsigned long int len) {
  memory_debug_printf("Allocating %lu bytes", len);

  if(len < 1) {
    memory_debug_printf("Refusing to allocate %lu bytes", len);
    return NULL;
  }

  void* toRet = malloc(len);

  if(!is_valid_ptr(toRet)) {
    memory_debug_printf("Failed to allocate %lu bytes!", len);
    return NULL;
  }

  memory_debug_printf("Successfully allocated %lu bytes", len);
  int failed = tAdd(toRet, len);
  if(failed) {
    memory_debug_print("Could not add pointer to list!");
    free(toRet);
    return NULL;
  }

  memory_debug_print("Successfully added pointer to list");
  tPrintStatus();

  return toRet;
}

// Free the given pointer and remove it from the list,
// if it is in the list and is a valid pointer
int tFree(void* ptr) {
  if(!is_valid_ptr(ptr)) return 1; // invalid pointer

  memory_debug_print("Attempting to free a pointer");

  // Find the pointer in the list
  // if we don't have it, then we don't manage it
  long int spot = tFindSpot(ptr);
  if(spot < 0) {
    memory_debug_print("Could not find pointer in list!");
    return 1;
  }

  struct ptr_data* ptrData = &POINTER_LIST[spot];

  // Free the pointer, remove it from the list
  free(ptr);
  ptrData->ptr = NULL;
  memory_debug_printf("Freed %lu bytes", ptrData->size);
  ptrData->size = 0;
  tPrintStatus();
  return 0;
}

#undef MEMORY_DEBUG
#undef memory_debug_print
#undef memory_debug_printf

#endif
