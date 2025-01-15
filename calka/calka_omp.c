#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

#define NUM_REPEATS 10  

double funkcja(double x) { 
    return(x*x*x); 
}

int main(int argc, char *argv[]) {
    printf("\nProgram obliczania całki metodą trapezów.\n");

    double a = 0.0;
    double b = 1.0;
    int N = 100000000;
    double dx_adjust = (b-a)/N;
    
    //tablica na czasy
    double times[NUM_REPEATS];
    double calka = 0.0;
    
    printf("\nRozpoczynam %d powtórzeń obliczeń OpenMP\n", NUM_REPEATS);
    
    
    for(int repeat = 0; repeat < NUM_REPEATS; repeat++) {
        calka = 0.0;  
        
        double t1 = omp_get_wtime();
        
        #pragma omp parallel for default(none) firstprivate(N, a, dx_adjust) reduction(+:calka)
        for(int i=0; i<N; i++) {
            double x1 = a + i*dx_adjust;
            calka += 0.5*dx_adjust*(funkcja(x1)+funkcja(x1+dx_adjust));
        }
        
        times[repeat] = omp_get_wtime() - t1;
        printf("Powtórzenie %d: Czas wykonania %lf. Obliczona całka = %.15lf\n", 
               repeat+1, times[repeat], calka);
    }
    
    // Obliczenie średniego czasu (z pominięciem wartości odstających)
    double sum = 0.0;
    double min_time = times[0];
    double max_time = times[0];
    int valid_measurements = NUM_REPEATS;
    
    for(int i = 0; i < NUM_REPEATS; i++) {
        if(times[i] < min_time) min_time = times[i];
        if(times[i] > max_time) max_time = times[i];
        sum += times[i];
    }
    
    // Usunięcie skrajnych wartości z obliczenia średniej
    sum = sum - min_time - max_time;
    valid_measurements -= 2;
    
    double avg_time = sum / valid_measurements;
    printf("\nStatystyki czasów wykonania:\n");
    printf("Średni czas (bez skrajnych): %lf\n", avg_time);
    printf("Najlepszy czas: %lf\n", min_time);
    printf("Najgorszy czas: %lf\n", max_time);
    
    return 0;
}