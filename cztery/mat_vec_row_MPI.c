#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define WYMIAR 10000
#define ROZMIAR (WYMIAR*WYMIAR)

void mat_vec(double* a, double* x, double* y, int n, int nt);

int main(int argc, char** argv) {
    double *x, *y, *z, *a;
    double t1;
    int n, nt, i, j;
    
    int rank, size, root = 1;
    int n_wier, n_wier_last;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if(size < 2) {
        printf("Program wymaga minimum 2 procesów MPI!\n");
        MPI_Finalize();
        exit(1);
    }
    
    n = WYMIAR;
    
    // Alokacja pamięci dla wszystkich procesów
    x = (double*)malloc(WYMIAR * sizeof(double));
    y = (double*)malloc(WYMIAR * sizeof(double));
    z = (double*)malloc(WYMIAR * sizeof(double));
    
    if (x == NULL || y == NULL || z == NULL) {
        printf("Błąd alokacji pamięci!\n");
        MPI_Finalize();
        exit(1);
    }
    
    // Inicjalizacja x zerami dla wszystkich procesów
    for(i = 0; i < WYMIAR; i++) {
        x[i] = 0.0;
        y[i] = 0.0;
        z[i] = 0.0;
    }
    
    // Tylko proces root alokuje i inicjalizuje macierz a
    if(rank == root) {
        a = (double*)malloc(ROZMIAR * sizeof(double));
        if (a == NULL) {
            printf("Błąd alokacji pamięci dla macierzy a!\n");
            MPI_Finalize();
            exit(1);
        }
        
        // Inicjalizacja danych
        for(i = 0; i < ROZMIAR; i++) a[i] = 1.0 * i;
        for(i = 0; i < WYMIAR; i++) x[i] = 1.0 * (WYMIAR - i);
        
        nt = 1;
        printf("poczatek (wykonanie sekwencyjne)\n");
        
        t1 = MPI_Wtime();
        mat_vec(a, x, y, n, nt);
        t1 = MPI_Wtime() - t1;
        
        printf("\tczas wykonania (sekwencyjne): %lf, Gflop/s: %lf, GB/s> %lf\n",
               t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
    }
    
    // Broadcast wektora x do wszystkich procesów
    MPI_Bcast(x, WYMIAR, MPI_DOUBLE, root, MPI_COMM_WORLD);
    
    // Obliczenie liczby wierszy dla każdego procesu
    n_wier = WYMIAR / size;
    n_wier_last = WYMIAR - n_wier * (size - 1);
    
    if(n_wier != n_wier_last) {
        if(rank == 0) printf("Ta wersja wymaga, aby WYMIAR był podzielny przez liczbę procesów!\n");
        MPI_Finalize();
        exit(0);
    }
    
    // Alokacja pamięci dla lokalnej części macierzy
    double *a_local = (double*)malloc(WYMIAR * n_wier * sizeof(double));
    if (a_local == NULL) {
        printf("Błąd alokacji pamięci dla a_local!\n");
        MPI_Finalize();
        exit(1);
    }
    
    if(rank == root) {
        t1 = MPI_Wtime();
    }
    
    // Rozproszenie macierzy A z użyciem MPI_IN_PLACE
    MPI_Scatter(rank == root ? a : NULL, n_wier * WYMIAR, MPI_DOUBLE,
                a_local, n_wier * WYMIAR, MPI_DOUBLE,
                root, MPI_COMM_WORLD);
    
    // Mnożenie lokalne
    mat_vec(a_local, x, z, n_wier, 1);
    
    // Zebranie wyników z użyciem MPI_IN_PLACE
    MPI_Gather(rank == root ? MPI_IN_PLACE : z, n_wier, MPI_DOUBLE,
               z, n_wier, MPI_DOUBLE,
               root, MPI_COMM_WORLD);
    
    if(rank == root) {
        t1 = MPI_Wtime() - t1;
        printf("\tczas wykonania (równoległe): %lf, Gflop/s: %lf, GB/s> %lf\n",
               t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
               
        // Weryfikacja wyników
        double max_diff = 0.0;
        for(i = 0; i < WYMIAR; i++) {
            max_diff = fmax(max_diff, fabs(y[i] - z[i]));
        }
        printf("\tMaksymalna różnica między wersjami: %e\n", max_diff);
    }
    
    // Zwalnianie pamięci
    free(a_local);
    free(x);
    free(y);
    free(z);
    if(rank == root) free(a);
    
    MPI_Finalize();
    return 0;
}

void mat_vec(double *a, double *x, double *y, int n, int nt) {
    int i, j;
    for(i = 0; i < n; i++) {
        y[i] = 0.0;
        for(j = 0; j < WYMIAR; j++) {
            y[i] += a[i*WYMIAR + j] * x[j];
        }
    }
}