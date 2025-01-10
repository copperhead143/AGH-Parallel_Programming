#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 18

int main() {
    double a[WYMIAR];
    
    // Inicjalizacja tablicy równolegle
    #pragma omp parallel for default(none) shared(a) 
    for(int i = 0; i < WYMIAR; i++) {
        a[i] = 1.02 * i;
    }

    // Suma sekwencyjna (dla porównania)
    double suma = 0.0;
    for(int i = 0; i < WYMIAR; i++) {
        suma += a[i];
    }
    printf("Suma sekwencyjna: %lf\n", suma);

    // Suma równoległa z różnymi wersjami schedule
    double suma_parallel = 0.0;
    
    // wersja 1 => static, rozmiar porcji = 3
    printf("\nStatic, porcja=3\n");
    suma_parallel = 0.0;
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel) schedule(static, 3) ordered
    for(int i = 0; i < WYMIAR; i++) {
        #pragma omp ordered
        printf("a[%2d]->W_%1d  \n", i, omp_get_thread_num());
        suma_parallel += a[i];
    }
    printf("Suma (static, porcja=3): %lf\n", suma_parallel);

    // wersja 2 => static domyslny rozmiar porcji
    suma_parallel = 0.0;
    printf("\nStatic, domyslna porcja\n");
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel) schedule(static) ordered
    for(int i = 0; i < WYMIAR; i++) {
        #pragma omp ordered
        printf("a[%2d]->W_%1d  \n", i, omp_get_thread_num());
        suma_parallel += a[i];
    }
    printf("Suma (static, domyslna porcja): %lf\n", suma_parallel);

    // wersja 3 => dynamic rozmiar porcji = 2
    printf("\nDynamic, porcja=2\n");
    suma_parallel = 0.0;
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel) schedule(dynamic, 2) ordered
    for(int i = 0; i < WYMIAR; i++) {
        #pragma omp ordered
        printf("a[%2d]->W_%1d  \n", i, omp_get_thread_num());
        suma_parallel += a[i];
    }
    printf("Suma (dynamic, porcja=2): %lf\n", suma_parallel);

    // wersja 4 => dynamic domyslny rozmiar porcji
    printf("\nDynamic, domyslna porcja\n");
    suma_parallel = 0.0;
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel) schedule(dynamic) ordered
    for(int i = 0; i < WYMIAR; i++) {
        #pragma omp ordered
        printf("a[%2d]->W_%1d  \n", i, omp_get_thread_num());
        suma_parallel += a[i];
    }
    printf("Suma (dynamic, domyślna porcja): %lf\n", suma_parallel);

    return 0;
}