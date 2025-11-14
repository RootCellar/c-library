#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_DEBUG 0
#define REPLACE_MALLOC 1

#define DEFAULT_POINTER_LIST_SIZE 1024

#if REPLACE_MALLOC == 1
  #define malloc(len) tMalloc(len)
  #define free(ptr) tFree(ptr)
  #define realloc(ptr, len) tRealloc(ptr, len)
#endif

int is_valid_ptr(void* ptr);
int tOwnsAddress(void* adr);

long int tGetTotalAllocs();
unsigned long int tGetTotalAllocSize();
void tPrintStatus();

int tFreePointerList();

size_t tGetSize(void* ptr);
int tHasPtr(void* ptr);

void tCondense();
int tResize(long int len);

void* tMalloc(unsigned long int len);
void* tRealloc(void* ptr, unsigned long int len);
void tFree(void* ptr);

#endif
