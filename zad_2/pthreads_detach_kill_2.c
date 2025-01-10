#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h> 

#define NUM_THREADS 5


void *thread_fun(void *arg) {
  int thread_id = *(int *)arg; // Rzutowanie argumentu na wskaźnik do int i przypisanie do zmiennej thread_id
  printf("thread %lu\n id %d\n", pthread_self(), thread_id); // Wypisanie identyfikatora wątku i jego id
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];
  int id[NUM_THREADS]; 
  int i;


  for (i = 0; i < NUM_THREADS; i++) {
    id[i] = i;
    pthread_create(&threads[i], NULL, thread_fun, (void *)&i);
  }

  // Oczekiwanie na zakończenie wątków
  for (i = 0; i < NUM_THREADS; i++) {
    if(pthread_join(threads[i], NULL) != 0){ // Oczekiwanie na zakończenie wątku
      printf("pthread_join error\n");
      exit(1);
    }
  }

  return 0;
}