#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 5

typedef struct{
    double start;
    double end;
    double result;
}data;

void *thread_fun(void * arg){
    data *d = (data *)arg;
    d->result = (d->start + d->end) / 2;
    pthread_exit(NULL);
}

int main(){

    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;
    data thread_data[NUM_THREADS];

    double a = 2.0;
    double b = 10.0;
    double interval = (b - a) / NUM_THREADS;

    for(int i = 0; i<NUM_THREADS; i++){
        //data *d = (data*)malloc(sizeof(data));
        thread_data[i].start = a + i * interval;
        thread_data[i].end = a + (i + 1) * interval;
        pthread_create(&threads[i], NULL, thread_fun, (void*)&thread_data[i]);
    }

    double result_total = 0.0;

    for(int i = 0; i<NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
        result_total += thread_data[i].result;
    }

    printf("Calkowity wynik: %f\n", result_total);

    return 0;

}