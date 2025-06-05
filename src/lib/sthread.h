#ifndef STHREAD_H
#define STHREAD_H

void* start_thread(void* args);
void run_in_threads(void (*function)(size_t, size_t, size_t, void*), void* items, size_t count, size_t thread_count);

#endif //STHREAD_H
