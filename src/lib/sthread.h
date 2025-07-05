#ifndef STHREAD_H
#define STHREAD_H

#include <pthread.h>

struct async_thread_args {
    void* input;
    void (*function)(void*);
};

struct async_exec_data {
    pthread_t thread;
    struct async_thread_args* thread_args;

    int errorCode = 0;
};

void* start_thread(void* args);
int run_in_threads(void (*function)(size_t, size_t, size_t, void*), void* items, size_t count, size_t thread_count);

struct async_exec_data run_asynchronous(void (*)(void*), void*);
void await_async_function(struct async_exec_data);

#endif //STHREAD_H
