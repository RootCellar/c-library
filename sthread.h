
#ifndef STHREAD_H
#define STHREAD_H

#include <pthread.h>

struct thread_args {
  int id;
  int thread_count;
  void* items;
  int count;
  void (*function)(int,int,void*);
};

void start_thread(void* args) {
  struct thread_args* thread_data = (struct thread_args*) (args);
  (thread_data->function)(thread_data->id, thread_data->thread_count, thread_data->items);
  pthread_exit(0);
}

void run_in_threads(void* function, void* items, int count, int thread_count) {

  pthread_t threads[thread_count];
  struct thread_args args[thread_count];

  for(int i = 0; i < thread_count; i++) {
    args[i].id = i;
    args[i].thread_count = thread_count;
    args[i].items = items;
    args[i].count = count;
    args[i].function = function;

    pthread_create(&threads[i], NULL, start_thread, (void*)&(args[i]));
  }

  for(int i = 0; i < thread_count; i++)
  {
    pthread_join(threads[i], NULL);
  }

}

#endif //STHREAD_H
