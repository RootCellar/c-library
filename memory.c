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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "memory.h"
#include "debug.h"

// Memory debug prints used only in this file
#define memory_debug_printf(fmt, ...) if(MEMORY_DEBUG) debug_printf(fmt, __VA_ARGS__);
#define memory_debug_print(fmt) if(MEMORY_DEBUG) debug_print(fmt);

// Undefine malloc and free macros so we use the real functions
#undef malloc
#undef free

struct ptr_data {
  char* ptr;
  size_t size;
};

static
struct ptr_data* POINTER_LIST = NULL;

static
long int POINTER_LIST_SIZE = 0;

int is_valid_ptr(void* ptr) {
  return ptr != NULL;
}

// Determines whether or not the given address is owned
// based on what's in the pointer list
int tOwnsAddress(void* adr) {
  if(!is_valid_ptr(POINTER_LIST)) return 0;

  struct ptr_data* current;

  uintptr_t addr_uint = (uintptr_t) adr;
  uintptr_t ptr_uint;

  for(long int i = 0; i < POINTER_LIST_SIZE; i++) {
    if(is_valid_ptr(POINTER_LIST[i].ptr)) {
      current = &POINTER_LIST[i];
      ptr_uint = (uintptr_t) current->ptr;

      if(ptr_uint <= addr_uint &&
        addr_uint < ptr_uint + current->size) {
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
  memory_debug_printf("The pointer list is %zu bytes, to hold %ld pointers.",
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
static
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

int tHasPtr(void* ptr) {
  return tFindSpot(ptr) >= 0;
}

// Get the size of the given pointer.
// Must be kept tracked of in the list.
// Returns -1 if the pointer is not found.
size_t tGetSize(void* ptr) {
  long int spot = tFindSpot(ptr);
  if(spot < 0) { return 0; }

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
static
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

// Reallocate the given pointer and update the list entry
// If the block of memory is expanded, the new space is
// filled with zeros
void* tRealloc(void* ptr, unsigned long int len) {
  memory_debug_printf("Reallocating %p to %lu bytes", ptr, len);

  if(!is_valid_ptr(ptr)) return NULL;

  long int spot = tFindSpot(ptr);
  if(spot < 0) {
    memory_debug_print("Could not find pointer in the list!");
    return NULL;
  }

  struct ptr_data* ptr_data = &POINTER_LIST[spot];
  int expanded = 0;
  if(len > ptr_data->size) expanded = 1;

  void* new_ptr = realloc(ptr, len);
  if(!is_valid_ptr(new_ptr)) {
    memory_debug_print("Could not reallocate pointer!");
    return NULL;
  }

  if(expanded) {
    memory_debug_print("Memory block was expanded, zero-filling new space...");
    memset((char*)new_ptr + ptr_data->size, 0, len - ptr_data->size);
  }

  memory_debug_print("Updating list entry...");

  ptr_data->ptr = new_ptr;
  ptr_data->size = len;

  return new_ptr;
}

// Free the given pointer and remove it from the list,
// if it is in the list and is a valid pointer
void tFree(void* ptr) {
  if(!is_valid_ptr(ptr)) {
    // invalid pointer
    memory_debug_print("Not freeing an invalid pointer");
    return;
  }

  memory_debug_print("Attempting to free a pointer");

  // Find the pointer in the list
  // if we don't have it, then we don't manage it
  long int spot = tFindSpot(ptr);
  if(spot < 0) {
    memory_debug_print("Could not find pointer in list!");
    return;
  }

  struct ptr_data* ptrData = &POINTER_LIST[spot];

  // Free the pointer, remove it from the list
  free(ptr);
  ptrData->ptr = NULL;
  memory_debug_printf("Freed %zu bytes", ptrData->size);
  ptrData->size = 0;
  tPrintStatus();
  return;
}
