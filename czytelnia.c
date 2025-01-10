#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#include"czytelnia.h"


void inicjuj(cz_t* cz_p){
  cz_p->l_p = 0;
  cz_p->l_c = 0;  
  cz_p->czeka_p = 0;
  pthread_mutex_init(&cz_p->mutex, NULL);
  pthread_cond_init(&cz_p->pisarze, NULL);
  pthread_cond_init(&cz_p->czytelnicy, NULL);
}

/*** Implementacja procedur interfejsu ***/

int my_read_lock_lock(cz_t* cz_p){

  pthread_mutex_lock(&cz_p->mutex);
  //czekanie czy jest pisarz lub jakis czekajacy pisarz

  while(cz_p->l_p > 0 || cz_p->czeka_p > 0){
    pthread_cond_wait(&cz_p->czytelnicy, &cz_p->mutex);
  }

  cz_p->l_c++;
  pthread_mutex_unlock(&cz_p->mutex);
  return 0;
}


int my_read_lock_unlock(cz_t* cz_p){
  pthread_mutex_lock(&cz_p->mutex);

  cz_p->l_c--;

  //jesli ostatni czytelkik wychodzi, sygnalizuj pisarzon
  if(cz_p->l_c == 0){
    pthread_cond_signal(&cz_p->pisarze);
  }

  pthread_mutex_unlock(&cz_p->mutex);
  return 0;
}


int my_write_lock_lock(cz_t* cz_p) {
    pthread_mutex_lock(&cz_p->mutex);
    cz_p->czeka_p++;
    // Czekamy jeśli są czytelnicy lub inny pisarz
    while (cz_p->l_c > 0 || cz_p->l_p > 0) {
        pthread_cond_wait(&cz_p->pisarze, &cz_p->mutex);
    }
    cz_p->czeka_p--;
    cz_p->l_p++;
    pthread_mutex_unlock(&cz_p->mutex);
    return 0;
}

int my_write_lock_unlock(cz_t* cz_p) {
    pthread_mutex_lock(&cz_p->mutex);
    cz_p->l_p--;
    // Sygnalizuj wszystkim czekającym
    pthread_cond_broadcast(&cz_p->czytelnicy);
    pthread_cond_signal(&cz_p->pisarze);
    pthread_mutex_unlock(&cz_p->mutex);
    return 0;
}

void czytam(cz_t* cz_p) {
#ifdef MY_DEBUG
    printf("\t\t\t\t\tczytam:  l_c %d, l_p %d, czeka_p %d\n", 
           cz_p->l_c, cz_p->l_p, cz_p->czeka_p);
    
    if (cz_p->l_p > 1 || (cz_p->l_p == 1 && cz_p->l_c > 0) || 
        cz_p->l_p < 0 || cz_p->l_c < 0) {
        printf("Błąd: Naruszenie zasad działania czytelni!\n");
        printf("l_p: %d, l_c: %d\n", cz_p->l_p, cz_p->l_c);
        exit(1);
    }
#endif
    usleep(rand() % 3000000);
}

void pisze(cz_t* cz_p) {
#ifdef MY_DEBUG
    printf("\t\t\t\t\tpisze:   l_c %d, l_p %d, czeka_p %d\n", 
           cz_p->l_c, cz_p->l_p, cz_p->czeka_p);
    
    if (cz_p->l_p > 1 || (cz_p->l_p == 1 && cz_p->l_c > 0) || 
        cz_p->l_p < 0 || cz_p->l_c < 0) {
        printf("Błąd: Naruszenie zasad działania czytelni!\n");
        printf("l_p: %d, l_c: %d\n", cz_p->l_p, cz_p->l_c);
        exit(1);
    }
#endif
    usleep(rand() % 3000000);
}


