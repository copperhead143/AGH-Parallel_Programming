#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define WYMIAR 10080 
#define ROZMIAR (WYMIAR*WYMIAR)
#define NUM_REPEATS 5  // Liczba powtórzeń pomiarów

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
    
    // Inicjalizacja x
    for(i=0; i<WYMIAR; i++) x[i]=0.0;
    
    if(rank==0) {
        a = (double *) malloc((ROZMIAR+1)*sizeof(double));
        for(i=0; i<ROZMIAR; i++) a[i]=1.0*i;
        for(i=0; i<WYMIAR; i++) x[i]=1.0*(WYMIAR-i);
        mat_vec(a, x, y, n, nt);
    }
    
    if(size>0) {
        // Inicjalizacja z
        for(i=0; i<WYMIAR; i++) z[i]=0.0;
        
        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        n_wier = ceil(WYMIAR / size);
        n_wier_last = WYMIAR - n_wier*(size-1);
        
        if(n_wier!=n_wier_last) {
            if(rank==0) printf("This version does not work with WYMIAR not a multiple of size!\n");
            MPI_Finalize();
            exit(0);
        }
        
        double *a_local = (double *) malloc(WYMIAR*n_wier*sizeof(double));
        for(i=0; i<WYMIAR*n_wier; i++) a_local[i]=0.0;
        
        // Tablica na czasy pomiarów
        double times[NUM_REPEATS];
        
        if(rank==0) printf("Starting %d repetitions of MPI matrix-vector product!\n", NUM_REPEATS);
        
        // Wykonanie kilku powtórzeń
        for(int repeat = 0; repeat < NUM_REPEATS; repeat++) {
            MPI_Scatter(a, n_wier*WYMIAR, MPI_DOUBLE, a_local, n_wier*WYMIAR, 
                       MPI_DOUBLE, 0, MPI_COMM_WORLD);
            MPI_Bcast(x, WYMIAR, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            
            // Synchronizacja przed pomiarem
            MPI_Barrier(MPI_COMM_WORLD);
            
            if(rank==0) t1 = MPI_Wtime();
            
            MPI_Allgather(&x[rank*n_wier], n_wier, MPI_DOUBLE, x, n_wier, 
                         MPI_DOUBLE, MPI_COMM_WORLD);
            
            for(i=0; i<n_wier; i++) {
                double t=0.0;
                int ni = n*i;
                for(j=0; j<n; j++) {
                    t += a_local[ni+j]*x[j];
                }
                z[i]=t;
            }
            
            // Synchronizacja po obliczeniach
            MPI_Barrier(MPI_COMM_WORLD);
            
            if(rank==0) {
                times[repeat] = MPI_Wtime() - t1;
                printf("Repetition %d: Time: %lf\n", repeat+1, times[repeat]);
            }
            
            MPI_Gather(z, n_wier, MPI_DOUBLE, z, n_wier, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            
            // Weryfikacja wyników tylko w ostatnim powtórzeniu
            if(repeat == NUM_REPEATS-1 && rank==0) {
                for(i=0; i<WYMIAR; i++) {
                    if(fabs(y[i]-z[i])>1.e-9*z[i]) {
                        printf("Błąd! i=%d, y[i]=%lf, z[i]=%lf\n", i, y[i], z[i]);
                    }
                }
            }
        }
        
        // Obliczenie statystyk czasów tylko dla rank 0
        if(rank==0) {
            double sum = 0.0;
            double min_time = times[0];
            double max_time = times[0];
            int valid_measurements = NUM_REPEATS;
            
            for(i = 0; i < NUM_REPEATS; i++) {
                if(times[i] < min_time) min_time = times[i];
                if(times[i] > max_time) max_time = times[i];
                sum += times[i];
            }
            
            // Usunięcie skrajnych wartości
            sum = sum - min_time - max_time;
            valid_measurements -= 2;
            
            double avg_time = sum / valid_measurements;
            printf("\nStatystyki czasów wykonania:\n");
            printf("Średni czas (bez skrajnych): %lf\n", avg_time);
            printf("Najlepszy czas: %lf\n", min_time);
            printf("Najgorszy czas: %lf\n", max_time);
            printf("Gflop/s: %lf, GB/s: %lf\n", 
                   2.0e-9*ROZMIAR/avg_time, 
                   (1.0+1.0/n)*8.0e-9*ROZMIAR/avg_time);
        }
        
        free(a_local);
    }
    
    if(rank==0) free(a);
    
    MPI_Finalize();
    return 0;
}

void mat_vec(double* a, double* x, double* y, int nn, int nt) {
    register int k=0;
    register int n=nn;
    register int i;
    register int j;
    
    for(i=0; i<n; i+=2) {
        register double ty1 = 0;
        register double ty2 = 0;
        for(j=0; j<n; j+=2) {
            register double t0=x[j];
            register double t1=x[j+1];
            k= i*n+j;
            ty1  +=a[k]*t0    +a[k+1]*t1;
            ty2  +=a[k+n]*t0  +a[k+1+n]*t1;
        }
        y[i]  = ty1;
        y[i+1]+=ty2;
    }
}