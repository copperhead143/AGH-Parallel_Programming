#include <stdio.h>
#include <pthread.h>
#include <math.h>

double funkcja(double x);
double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w);

static int l_w_global = 0;
static double calka_global = 0.0;
static double a_global;
static double b_global;
static double dx_global;
static int N_global;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* calka_fragment_petli_w(void* arg_wsk);

double calka_zrownoleglenie_petli(double a, double b, double dx, int l_w) {
    int N = ceil((b - a) / dx);
    double dx_adjust = (b - a) / N;

    printf("Obliczona liczba trapezów: N = %d, dx_adjust = %lf\n", N, dx_adjust);

    // Ustawienia globalne
    a_global = a;
    b_global = b;
    dx_global = dx_adjust;
    N_global = N;
    l_w_global = l_w;
    calka_global = 0.0;

    pthread_t threads[l_w];
    int thread_ids[l_w];

    // Tworzenie wątków
    for (int i = 0; i < l_w; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, calka_fragment_petli_w, (void*)&thread_ids[i]);
    }

    // Oczekiwanie na zakończenie pracy wątków
    for (int i = 0; i < l_w; i++) {
        pthread_join(threads[i], NULL);
    }

    return calka_global;
}

//dekompozycja cykliczna
void* calka_fragment_petli_w(void* arg_wsk) {
    int my_id = *(int*)arg_wsk;
    double calka_local = 0.0;


    // Cykliczna dekompozycja
    for (int i = my_id; i < N_global; i += l_w_global) {
        double x1 = a_global + i * dx_global;
        calka_local += 0.5 * dx_global * (funkcja(x1) + funkcja(x1 + dx_global));
    }

    // Redukcja wyników wątku
    pthread_mutex_lock(&mutex);
    calka_global += calka_local;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

/* Alternatywna dekompozycja blokowa */
void* calka_fragment_petli_w(void* arg_wsk) {
    int my_id = *(int*)arg_wsk;
    int my_start = my_id * (N_global / l_w_global);
    int my_end = (my_id == l_w_global - 1) ? N_global : my_start + (N_global / l_w_global);
    double calka_local = 0.0;


    for (int i = my_start; i < my_end; i++) {
        double x1 = a_global + i * dx_global;
        calka_local += 0.5 * dx_global * (funkcja(x1) + funkcja(x1 + dx_global));
    }

    pthread_mutex_lock(&mutex);
    calka_global += calka_local;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

//do sprawka wyniki z pthreads_suma
