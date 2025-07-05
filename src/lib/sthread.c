#include <pthread.h>
#include <stdlib.h>

#include "sthread.h"

struct thread_args {
  size_t id;
  size_t count;
  size_t thread_count;
  void* items;
  void (*function)(size_t, size_t, size_t, void*);

  pthread_mutex_t mutex;
  int goAhead;
};

void* start_thread(void* args) {
  struct thread_args* thread_data = (struct thread_args*) (args);

  pthread_mutex_lock(&thread_data->mutex);

  if(thread_data->goAhead) {
    (thread_data->function)(thread_data->id, thread_data->thread_count, thread_data->count, thread_data->items);
  }

  pthread_mutex_unlock(&thread_data->mutex);

  pthread_exit(0);
}

int run_in_threads(void (*function)(size_t, size_t, size_t, void*), void* items, size_t count, size_t thread_count) {
  pthread_t threads[thread_count];
  struct thread_args args[thread_count];

  int pthread_create_error = 0;

  for(size_t i = 0; i < thread_count; i++) {
    args[i].id = i;
    args[i].thread_count = thread_count;
    args[i].items = items;
    args[i].count = count;
    args[i].function = function;

    // Lock the thread argument mutex
    pthread_mutex_init(&args[i].mutex, NULL);
    pthread_mutex_lock(&args[i].mutex);

    // Assume there will be an error, tell threads not to actually run
    args[i].goAhead = 0;

    // Create a thread, which also starts it.
    // If there is an error making the thread, acknowledge it
    int result = pthread_create(&threads[i], NULL, start_thread, (void*) &(args[i]));
    if(result != 0) {
      pthread_create_error = 1;
    }
  }

  // If there weren't any errors while making threads,
  // tell them it's OK to run
  if(!pthread_create_error) {
    for(size_t i = 0; i < thread_count; i++) {
      args[i].goAhead = 1;
    }
  }

  // Uncnoditionally unlock the threads' mutexes
  for(size_t i = 0; i < thread_count; i++) {
    pthread_mutex_unlock(&args[i].mutex);
  }

  // Wait until all of the threads finish
  for(size_t i = 0; i < thread_count; i++) {
    pthread_join(threads[i], NULL);
  }

  // Destroy the mutexes
  for(size_t i = 0; i < thread_count; i++) {
    pthread_mutex_destroy(&args[i].mutex);
  }

  if(pthread_create_error) return 1;
  return 0;
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

  int errorCode = pthread_create(&toRet.thread, NULL, execute_async, (void*) toRet.thread_args);

  toRet.errorCode = errorCode;

  return toRet;
}

void await_async_function(struct async_exec_data data) {
  pthread_join(data.thread, NULL);
  free(data.thread_args);
}
