#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define ROZMIAR_1 1000
#define ROZMIAR_2 100000000
#define LICZBA_W 2 // Liczba wątków w wersji równoległej

pthread_t watki[LICZBA_W];
double global_array_of_local_sums[LICZBA_W];
double *tab;
double suma = 0; 
pthread_mutex_t muteks;

typedef struct {
    int moj_id;
    int rozmiar;
} ParametryWątku;

double czas_zegara() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1000000.0;
}

// Sekwencyjna wersja obliczania sumy
void oblicz_sekwencyjnie(int rozmiar) {
    suma = 0;
    for (int i = 0; i < rozmiar; i++) {
        suma += tab[i];
    }
}

// Funkcja wątku obliczająca sumę w wersji równoległej z mutexem
void *suma_w(void *arg_wsk) {
    ParametryWątku *parametry = (ParametryWątku *)arg_wsk;
    int moj_id = parametry->moj_id;
    int rozmiar = parametry->rozmiar;
    int j = ceil((float)rozmiar / LICZBA_W);
    double moja_suma = 0;

    for (int i = j * moj_id; i < j * (moj_id + 1); i++) {
        if (i >= rozmiar) break;  // Zapobieganie przekroczeniu zakresu
        moja_suma += tab[i];
    }

    pthread_mutex_lock(&muteks);
    suma += moja_suma;
    pthread_mutex_unlock(&muteks);

    pthread_exit(NULL);
}

// Funkcja równoległa bez mutexu
void *suma_w_no_mutex(void *arg_wsk) {
    ParametryWątku *parametry = (ParametryWątku *)arg_wsk;
    int moj_id = parametry->moj_id;
    int rozmiar = parametry->rozmiar;
    int j = ceil((float)rozmiar / LICZBA_W);
    double tmp = 0.0;

    for (int i = j * moj_id; i < j * (moj_id + 1); i++) {
        if (i >= rozmiar) break;  // Zapobieganie przekroczeniu zakresu
        tmp += tab[i];
    }

    global_array_of_local_sums[moj_id] = tmp;
    pthread_exit(NULL);
}

void oblicz_rownolegle(int rozmiar) {
    pthread_mutex_init(&muteks, NULL);
    ParametryWątku parametry[LICZBA_W];
    
    for (int i = 0; i < LICZBA_W; i++) {
        parametry[i].moj_id = i;
        parametry[i].rozmiar = rozmiar;
    }

    suma = 0;
    for (int i = 0; i < LICZBA_W; i++) {
        pthread_create(&watki[i], NULL, suma_w, (void *)&parametry[i]);
    }

    for (int i = 0; i < LICZBA_W; i++) {
        pthread_join(watki[i], NULL);
    }

    pthread_mutex_destroy(&muteks);
}

void oblicz_rownolegle_no_mutex(int rozmiar) {
    pthread_mutex_init(&muteks, NULL);
    ParametryWątku parametry[LICZBA_W];

    for (int i = 0; i < LICZBA_W; i++) {
        parametry[i].moj_id = i;
        parametry[i].rozmiar = rozmiar;
    }

    suma = 0;
    for (int i = 0; i < LICZBA_W; i++) {
        pthread_create(&watki[i], NULL, suma_w_no_mutex, (void *)&parametry[i]);
    }

    for (int i = 0; i < LICZBA_W; i++) {
        pthread_join(watki[i], NULL);
        suma += global_array_of_local_sums[i];
    }

    pthread_mutex_destroy(&muteks);
}

int main() {
    // Porównanie dla mniejszych rozmiarów
    printf("Porównanie dla rozmiaru 1000\n");

    tab = (double *)malloc(ROZMIAR_1 * sizeof(double));
    if (tab == NULL) {
        fprintf(stderr, "Błąd alokacji pamięci!\n");
        exit(1);
    }

    for (int i = 0; i < ROZMIAR_1; i++) {
        tab[i] = ((double)i + 1) / ROZMIAR_1;
    }

    double t1 = czas_zegara();
    oblicz_sekwencyjnie(ROZMIAR_1);
    t1 = czas_zegara() - t1;
    printf("Sekwencyjnie suma = %lf\n", suma);
    printf("Czas obliczeń sekwencyjnych: %lf\n", t1);

    t1 = czas_zegara();
    oblicz_rownolegle(ROZMIAR_1);
    t1 = czas_zegara() - t1;
    printf("Równolegle suma = %lf\n", suma);
    printf("Czas obliczeń równoległych (z mutexem): %lf\n", t1);

    t1 = czas_zegara();
    oblicz_rownolegle_no_mutex(ROZMIAR_1);
    t1 = czas_zegara() - t1;
    printf("Równolegle suma (bez mutexu) = %lf\n", suma);
    printf("Czas obliczeń równoległych (bez mutexu): %lf\n", t1);

    free(tab);

    // Porównanie dla dużych rozmiarów
    printf("\nPorównanie dla rozmiaru 100000000\n");

    tab = (double *)malloc(ROZMIAR_2 * sizeof(double));
    if (tab == NULL) {
        fprintf(stderr, "Błąd alokacji pamięci!\n");
        exit(1);
    }

    for (int i = 0; i < ROZMIAR_2; i++) {
        tab[i] = ((double)i + 1) / ROZMIAR_2;
    }

    t1 = czas_zegara();
    oblicz_sekwencyjnie(ROZMIAR_2);
    t1 = czas_zegara() - t1;
    printf("Sekwencyjnie suma = %lf\n", suma);
    printf("Czas obliczeń sekwencyjnych: %lf\n", t1);

    t1 = czas_zegara();
    oblicz_rownolegle(ROZMIAR_2);
    t1 = czas_zegara() - t1;
    printf("Równolegle suma = %lf\n", suma);
    printf("Czas obliczeń równoległych (z mutexem): %lf\n", t1);

    t1 = czas_zegara();
    oblicz_rownolegle_no_mutex(ROZMIAR_2);
    t1 = czas_zegara() - t1;
    printf("Równolegle suma (bez mutexu) = %lf\n", suma);
    printf("Czas obliczeń równoległych (bez mutexu): %lf\n", t1);

    free(tab);
    return 0;
}
