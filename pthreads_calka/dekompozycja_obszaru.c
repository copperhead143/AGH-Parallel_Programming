#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ThreadData {
  int my_id;
  double a;
  double b;
  double dx;
  double calka;
} ThreadData;

double funkcja(double x);

double calka_sekw(double a, double b, double dx);

double calka_dekompozycja_obszaru(double a, double b, double dx, int l_w);

void *calka_podobszar_w(void *arg_wsk);

double calka_dekompozycja_obszaru(double a, double b, double dx, int l_w) {
  printf("a %lf, b %lf, dx %lf\n", a, b, dx);

  double calka_suma_local = 0.0;

  // tworzenie struktur danych do obsługi wielowątkowości
  ThreadData *thread_data = malloc(l_w * sizeof(ThreadData));
  pthread_t *thread_id = malloc(l_w * sizeof(pthread_t)); 

  double base_range = (b - a) / l_w;

  // tworzenie wątków
  for (int i = 0; i < l_w; ++i) {
    thread_data[i].my_id = i;
    thread_data[i].a = a + i * base_range;
    thread_data[i].b = a + (i + 1) * base_range;
    thread_data[i].dx = dx;
    thread_data[i].calka = 0.0;

    if (i == l_w - 1) {
      thread_data[i].b = b;
    }

    pthread_create(&thread_id[i], NULL, calka_podobszar_w,
                   (void *)&thread_data[i]);
  }

  // oczekiwanie na zakończenie pracy wątków
  for (int i = 0; i < l_w; ++i) {
    pthread_join(thread_id[i], NULL);
    calka_suma_local += thread_data[i].calka;
  }

  return (calka_suma_local);
}


void *calka_podobszar_w(void *arg_wsk) {
  ThreadData *thread_data = (ThreadData *)arg_wsk;

  double a_local = thread_data->a, b_local = thread_data->b,
         dx = thread_data->dx;
  // rozpakowanie danych przesłanych do wątku

  int my_id = thread_data->my_id; // skąd pobierany?
  printf("\nWątek %d: a_local %lf, b_local %lf, dx %lf\n", my_id, a_local,
         b_local, dx);
  // wywołanie zadania do wykonania: całkowanie w zadanym przedziale
  thread_data->calka = calka_sekw(a_local, b_local, dx);

}
