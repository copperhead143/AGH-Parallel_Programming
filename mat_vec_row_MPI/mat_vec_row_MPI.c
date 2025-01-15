#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

// Wymiar dla testów
#define WYMIAR 1000 // zmniejszone dla testów
#define ROZMIAR (WYMIAR*WYMIAR)

void mat_vec(double* a, double* x, double* y, int n, int nt);

int main(int argc, char** argv) {
    static double x[WYMIAR], y[WYMIAR], z[WYMIAR];
    double *a;
    double t1;
    int n, nt, i, j;
    
    int rank, size, source, dest, tag=0;
    int n_wier, n_wier_last;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    n = WYMIAR;
    
    // Inicjalizacja x zerami
    for(i=0; i<WYMIAR; i++) x[i]=0.0;
    
    if(rank==0) {
        a = (double*)malloc((ROZMIAR+1)*sizeof(double));
        
        // Inicjalizacja danych
        for(i=0; i<ROZMIAR; i++) a[i]=1.0*i;
        for(i=0; i<WYMIAR; i++) x[i]=1.0*(WYMIAR-i);
        
        nt=1;
        printf("poczatek (wykonanie sekwencyjne)\n");
        
        // Wykonanie sekwencyjne dla porównania
        t1 = MPI_Wtime();
        mat_vec(a,x,y,n,nt);
        t1 = MPI_Wtime() - t1;
        
        printf("\tczas wykonania (sekwencyjne): %lf, Gflop/s: %lf, GB/s> %lf\n",
               t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
    }
    
    if(size>1) {
        // Inicjalizacja z zerami
        for(i=0; i<WYMIAR; i++) z[i]=0.0;
        
        // Obliczenie rozmiaru części dla każdego procesu
        n_wier = WYMIAR / size;
        n_wier_last = WYMIAR - n_wier*(size-1);
        
        if(n_wier!=n_wier_last) {
            printf("Ta wersja wymaga, aby WYMIAR był podzielny przez liczbę procesów!\n");
            MPI_Finalize();
            exit(0);
        }
        
        // Alokacja pamięci dla lokalnej części macierzy
        double *a_local = (double*)malloc(WYMIAR*n_wier*sizeof(double));
        for(i=0; i<WYMIAR*n_wier; i++) a_local[i]=0.0;
        
        
        // Rozgłoszenie rozmiaru
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        // Rozproszenie macierzy A
        MPI_Scatter(a, n_wier*WYMIAR, MPI_DOUBLE,
                   a_local, n_wier*WYMIAR, MPI_DOUBLE,
                   0, MPI_COMM_WORLD);
        
        // Rozproszenie wektora x
        MPI_Scatter(x, n_wier, MPI_DOUBLE,
                   &x[rank*n_wier], n_wier, MPI_DOUBLE,
                   0, MPI_COMM_WORLD);
        
        // Rozgłoszenie całego wektora x
        MPI_Allgather(MPI_IN_PLACE, n_wier, MPI_DOUBLE,
                     x, n_wier, MPI_DOUBLE,
                     MPI_COMM_WORLD);
        
        if(rank==0) {
            t1 = MPI_Wtime();
        }
        
        // Obliczenia lokalne
        for(i=0; i<n_wier; i++) {
            double t=0.0;
            int ni = n*i;
            
            for(j=0; j<n; j++) {
                t += a_local[ni+j]*x[j];
            }
            z[i]=t;
        }
        
        // Zebranie wyników
        MPI_Gather(z, n_wier, MPI_DOUBLE,
                  z, n_wier, MPI_DOUBLE,
                  0, MPI_COMM_WORLD);
        
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        if(rank==0) {
            t1 = MPI_Wtime() - t1;
            printf("Wersja rownolegla MPI\n");
            printf("\tczas wykonania: %lf, Gflop/s: %lf, GB/s> %lf\n",
                   t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
            
            // Sprawdzenie poprawności
            for(i=0; i<WYMIAR; i++) {
                if(fabs(y[i]-z[i])>1.e-9*z[i]) {
                    printf("Blad! i=%d, y[i]=%lf, z[i]=%lf\n", i, y[i], z[i]);
                }
            }
        }
        
        free(a_local);
    }
    
    if(rank==0) {
        free(a);
    }
    
    MPI_Finalize();
    return 0;
}

// Funkcja do mnożenia macierz-wektor (wersja sekwencyjna)
void mat_vec(double* a, double* x, double* y, int nn, int nt) {
    register int n=nn;
    register int i;
    
    for(i=0; i<n; i+=2) {
        register double ty1 = 0;
        register double ty2 = 0;
        register int j;
        for(j=0; j<n; j+=2) {
            register double t0=x[j];
            register double t1=x[j+1];
            register int k= i*n+j;
            ty1 += a[k]*t0 + a[k+1]*t1;
            ty2 += a[k+n]*t0 + a[k+1+n]*t1;
        }
        y[i] = ty1;
        y[i+1] = ty2;
    }
}