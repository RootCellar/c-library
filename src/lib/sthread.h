#ifndef STHREAD_H
#define STHREAD_H

void* start_thread(void* args);
void run_in_threads(void* function, void* items, int count, int thread_count);

#endif //STHREAD_H
