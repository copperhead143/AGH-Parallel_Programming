#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define SCALAR double

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

int main(int argc, char** argv) {
    int max_liczba_wyrazow = 0;
    int rank, size;
    SCALAR suma_plus = 0.0;
    SCALAR suma_minus = 0.0;
    SCALAR global_suma_plus = 0.0;
    SCALAR global_suma_minus = 0.0;
    
    // Inicjalizacja MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Proces 0 wczytuje dane wejściowe
    if (rank == 0) {
        printf("Podaj maksymalną liczbę wyrazów do obliczenia przybliżenia PI\n");
        scanf("%d", &max_liczba_wyrazow);
    }
    
    // Rozgłoszenie liczby wyrazów do wszystkich procesów
    MPI_Bcast(&max_liczba_wyrazow, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Obliczenie zakresu dla każdego procesu (dekompozycja blokowa)
    int terms_per_process = max_liczba_wyrazow / size;
    int my_start = rank * terms_per_process;
    int my_end = (rank == size - 1) ? max_liczba_wyrazow : my_start + terms_per_process;
    
    // Pomiar czasu rozpoczęcia
    double t1 = MPI_Wtime();
    
    // Każdy proces oblicza swoją część
    for(int i = my_start; i < my_end; i++) {
        int j = 1 + 4 * i;
        suma_plus += 1.0 / j;
        suma_minus += 1.0 / (j + 2.0);
    }
    
    // Redukcja sum częściowych do wyniku końcowego
    MPI_Reduce(&suma_plus, &global_suma_plus, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&suma_minus, &global_suma_minus, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        double pi_approx = 4.0 * (global_suma_plus - global_suma_minus);
        double t = MPI_Wtime() - t1;
        
        printf("PI obliczone: \t\t\t%20.15lf\n", pi_approx);
        printf("PI z biblioteki matematycznej: \t%20.15lf\n", M_PI);
        printf("Czas obliczeń: %lf\n", t);
        printf("Błąd względny: %e\n", fabs(pi_approx - M_PI)/M_PI);
    }
    
    MPI_Finalize();
    return 0;
}