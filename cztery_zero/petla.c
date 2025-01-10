#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 10

int main() {
    double a[WYMIAR][WYMIAR];

    // Inicjalizacja tablicy
    for(int i = 0; i < WYMIAR; i++) 
        for(int j = 0; j < WYMIAR; j++) 
            a[i][j] = 1.02 * i + 1.01 * j;

    // Włączenie zagnieżdżonej równoległości
    omp_set_nested(1);

    // suma sekwencyjna dla porównania
    double suma = 0.0;
    for(int i = 0; i < WYMIAR; i++) 
        for(int j = 0; j < WYMIAR; j++) 
            suma += a[i][j];
    printf("Suma sekwencyjna: %lf\n", suma);

    //dekompozycja 2D nested
        printf("\ndekompozycja 2D nested\n");
    double suma_2d = 0.0;
    
    // Zewnętrzny obszar równoległy (wiersze) z 3 wątkami
    #pragma omp parallel num_threads(3) default(none) shared(a, suma_2d)
    {
        // Zmienna do przechowywania sumy lokalnej dla pierwszego poziomu wątkowania
        double suma_wiersz = 0.0;
        
        // Zmienna do przechowywania numeru wątku z pierwszego poziomu
        int id_wiersz = omp_get_thread_num();
        
        // Zewnętrzna pętla (po wierszach) z przydziałem statycznym, porcja 2
        #pragma omp for schedule(static, 2) 
        for(int i = 0; i < WYMIAR; i++) {
            // Wewnętrzny obszar równoległy (kolumny) z 2 wątkami
            #pragma omp parallel num_threads(2) default(none) \
                shared(a, i) firstprivate(id_wiersz) reduction(+:suma_wiersz)
            {
                // Numer wątku we wewnętrznym obszarze
                int id_kolumna = omp_get_thread_num();
                
                // Wewnętrzna pętla (po kolumnach) z przydziałem statycznym, porcja 2
                #pragma omp for schedule(static, 2) ordered
                for(int j = 0; j < WYMIAR; j++) {
                    #pragma omp ordered
                    printf("a[%1d][%1d] -> Wątek (%1d,%1d)\n", i, j, id_wiersz, id_kolumna);
                    suma_wiersz += a[i][j];
                }
            }
        }
        
        // Redukcja sumy całkowitej
        #pragma omp critical
        suma_2d += suma_wiersz;
    }
    
    printf("Suma dekompozycja 2D: %lf\n", suma_2d);

    // Dekompozycja 2D bez ustalonego rozmiaru porcji
    printf("\ndekompozycja 2D bez porcji\n");
    double suma_2d_bezporcji = 0.0;
    
    omp_set_nested(1);
    
    #pragma omp parallel num_threads(3) default(none) shared(a, suma_2d_bezporcji)
    {
        double suma_wiersz = 0.0;
        int id_wiersz = omp_get_thread_num();
        
        #pragma omp for schedule(static) 
        for(int i = 0; i < WYMIAR; i++) {
            #pragma omp parallel num_threads(2) default(none) \
                shared(a, i) firstprivate(id_wiersz) reduction(+:suma_wiersz)
            {
                int id_kolumna = omp_get_thread_num();
                
                #pragma omp for schedule(static) ordered
                for(int j = 0; j < WYMIAR; j++) {
                    #pragma omp ordered
                    printf("a[%1d][%1d] -> Wątek (%1d,%1d)\n", i, j, id_wiersz, id_kolumna);
                    suma_wiersz += a[i][j];
                }
            }
        }
        
        #pragma omp critical
        suma_2d_bezporcji += suma_wiersz;
    }
    
    printf("Suma dekompozycja 2D bez porcji: %lf\n", suma_2d_bezporcji);

    return 0;
}