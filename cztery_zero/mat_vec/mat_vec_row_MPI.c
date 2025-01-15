#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define NUM_ATTEMPTS 5
#define BASE_WYMIAR 3040  // W0 value from screenshot

// Function to calculate dimension for given workload multiplier
int calculate_wymiar(double workload_multiplier) {
    return (int)(BASE_WYMIAR * workload_multiplier);
}

double run_single_test(int wymiar, int rank, int size) {
    int rozmiar = wymiar * wymiar;
    double *x = (double*)malloc(wymiar * sizeof(double));
    double *y = (double*)malloc(wymiar * sizeof(double));
    double *z = (double*)malloc(wymiar * sizeof(double));
    double *a = NULL;
    
    if(rank == 0) {
        a = (double*)malloc(rozmiar * sizeof(double));
        for(int i = 0; i < rozmiar; i++) a[i] = 1.0 * i;
        for(int i = 0; i < wymiar; i++) x[i] = 1.0 * (wymiar - i);
    }
    
    int n_wier = wymiar / size;
    double *a_local = (double*)malloc(wymiar * n_wier * sizeof(double));
    
    MPI_Barrier(MPI_COMM_WORLD);
    double t1 = MPI_Wtime();
    
    MPI_Scatter(a, n_wier * wymiar, MPI_DOUBLE, a_local, n_wier * wymiar, 
                MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(x, wymiar, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    for(int i = 0; i < n_wier; i++) {
        double temp = 0.0;
        for(int j = 0; j < wymiar; j++) {
            temp += a_local[i * wymiar + j] * x[j];
        }
        z[i] = temp;
    }
    
    MPI_Gather(z, n_wier, MPI_DOUBLE, y, n_wier, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
    double t2 = MPI_Wtime();
    
    free(x);
    free(y);
    free(z);
    free(a_local);
    if(rank == 0 && a != NULL) free(a);
    
    return t2 - t1;
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Workload multipliers: W0, 2*W0, 4*W0, 8*W0
    double workloads[] = {1.0, 2.0, 4.0, 8.0};
    const char* workload_names[] = {"W0:", "2*W0:", "4*W0:", "8*W0:"};
    
    if(rank == 0) {
        printf("W0: %d\n", BASE_WYMIAR);
        printf("Number of threads: %d\n\n", size);
    }
    
    for(int w = 0; w < sizeof(workloads)/sizeof(workloads[0]); w++) {
        int wymiar = calculate_wymiar(workloads[w]);
        double times[NUM_ATTEMPTS];
        
        if(rank == 0) {
            printf("%s\n", workload_names[w]);
            printf("Attempt\tTime[s]\t\tGflop/s\t\tGB/s\n");
            printf("----------------------------------------\n");
        }
        
        for(int attempt = 0; attempt < NUM_ATTEMPTS; attempt++) {
            times[attempt] = run_single_test(wymiar, rank, size);
        }
        
        if(rank == 0) {
            double avg_time = 0.0;
            for(int i = 0; i < NUM_ATTEMPTS; i++) {
                avg_time += times[i];
            }
            avg_time /= NUM_ATTEMPTS;
            
            // Calculate metrics
            double gflops = 2.0e-9 * wymiar * wymiar / avg_time;  // GFlop/s
            double gbs = 8.0e-9 * wymiar * wymiar / avg_time;     // GB/s
            
            // Print individual attempts
            for(int i = 0; i < NUM_ATTEMPTS; i++) {
                printf("%d\t%.4f\t\t%.4f\t\t%.4f\n", 
                       i + 1, times[i], 
                       2.0e-9 * wymiar * wymiar / times[i],
                       8.0e-9 * wymiar * wymiar / times[i]);
            }
            
            // Print average
            printf("----------------------------------------\n");
            printf("AVG:\t%.4f\t\t%.4f\t\t%.4f\n", avg_time, gflops, gbs);
            printf("\n");
        }
    }
    
    MPI_Finalize();
    return 0;
}