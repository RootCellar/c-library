#include <pthread.h>
#include <stdlib.h>

#include "sthread.h"

struct thread_args {
  size_t id;
  size_t count;
  size_t thread_count;
  void* items;
  void (*function)(size_t, size_t, size_t, void*);
};

void* start_thread(void* args) {
  struct thread_args* thread_data = (struct thread_args*) (args);
  (thread_data->function)(thread_data->id, thread_data->thread_count, thread_data->count, thread_data->items);
  pthread_exit(0);
}

void run_in_threads(void (*function)(size_t, size_t, size_t, void*), void* items, size_t count, size_t thread_count) {
  pthread_t threads[thread_count];
  struct thread_args args[thread_count];

  for(size_t i = 0; i < thread_count; i++) {
    args[i].id = i;
    args[i].thread_count = thread_count;
    args[i].items = items;
    args[i].count = count;
    args[i].function = function;

    pthread_create(&threads[i], NULL, start_thread, (void*) &(args[i]));

    // TODO: pthread error handling?
  }

  for(size_t i = 0; i < thread_count; i++) {
    pthread_join(threads[i], NULL);
  }
}

static
void* execute_async(void* input) {
  struct async_thread_args* data = (struct async_thread_args*) input;

  (data->function)(data->input);

  pthread_exit(0);
}

struct async_exec_data run_asynchronous(void (*function)(void*), void* input) {
  struct async_exec_data toRet;

  toRet.thread_args = malloc(sizeof(struct async_thread_args));

  // TODO: malloc error handling
  
  toRet.thread_args->function = function;
  toRet.thread_args->input = input;

  pthread_create(&toRet.thread, NULL, execute_async, (void*) toRet.thread_args);

  // TODO: pthread error handling?

  return toRet;
}

void await_async_function(struct async_exec_data data) {
  pthread_join(data.thread, NULL);
  free(data.thread_args);
}
