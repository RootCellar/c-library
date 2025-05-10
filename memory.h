#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_DEBUG 0

#define DEFAULT_POINTER_LIST_SIZE 1024

int is_valid_ptr(void* ptr);
int tOwnsAddress(void* adr);

long int tGetTotalAllocs();
unsigned long int tGetTotalAllocSize();
void tPrintStatus();

int tFreePointerList();

long int tFindSpot(void* ptr);
size_t tGetSize(void* ptr);

void tCondense();
int tResize(long int len);

int tAdd(void* ptr, unsigned long int len);

void* tMalloc(unsigned long int len);
void* tRealloc(void* ptr, unsigned long int len);
void tFree(void* ptr);

#endif
