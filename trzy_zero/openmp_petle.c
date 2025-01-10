#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define WYMIAR 18

int main() {
    double a[WYMIAR][WYMIAR];
    double suma_kolumnowa = 0.0;

    // Initialize the array
    for(int i = 0; i < WYMIAR; i++) {
        for(int j = 0; j < WYMIAR; j++) {
            a[i][j] = 1.02 * (i + j);
        }
    }

    // 1. Column-wise sum with dynamic scheduling
    printf("suma kolumnowa dynamic schedule\n");
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_kolumnowa) schedule(dynamic) ordered
    for(int i = 0; i < WYMIAR; i++) {
        for(int j = 0; j < WYMIAR; j++) {
            #pragma omp ordered
            printf("a[%1d][%1d] -> Wątek %1d\n", i, j, omp_get_thread_num());
            suma_kolumnowa += a[i][j];
        }
    }
    printf("Suma kolumnowa: %lf\n", suma_kolumnowa);

    // 2. Row-wise sum with static scheduling
    printf("\nsuma wierszowa static schedule\n");
    double suma_wierszowa = 0.0;
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_wierszowa) schedule(static) ordered
    for(int i = 0; i < WYMIAR; i++) {
        for(int j = 0; j < WYMIAR; j++) {
            #pragma omp ordered
            printf("a[%1d][%1d] -> Wątek %1d\n", i, j, omp_get_thread_num());
            suma_wierszowa += a[i][j];
        }
    }
    printf("Suma wierszowa: %lf\n", suma_wierszowa);

    // 3. Alternative column-wise decomposition
    printf("alternatywna dekompozycja kolumnowa\n");
    double suma_reczna = 0.0;
    #pragma omp parallel default(none) shared(a, suma_reczna)
    {
        double suma_lokalna = 0.0;
        #pragma omp for schedule(static) nowait ordered
        for(int j = 0; j < WYMIAR; j++) {
            for(int i = 0; i < WYMIAR; i++) {
                #pragma omp ordered
                printf("a[%1d][%1d] -> Wątek %1d\n", i, j, omp_get_thread_num());
                suma_lokalna += a[i][j];
            }
        }
        
        #pragma omp critical
        suma_reczna += suma_lokalna;
    }
    printf("Suma ręczna: %lf\n", suma_reczna);

    return 0;
}