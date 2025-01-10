#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 1000000

void * watek_klient(void * arg);

// Zasoby pubu
int l_kf; // liczba dostępnych kufli
pthread_mutex_t kufle_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex do zabezpieczenia kufli

int main(void) {
    pthread_t *tab_klient;
    int *tab_klient_id;

    int l_kl, i;

    printf("\nLiczba klientow: "); 
    scanf("%d", &l_kl);

    printf("\nLiczba kufli: "); 
    scanf("%d", &l_kf);

    // Inicjalizacja tablic na wątki klientów i ich ID
    tab_klient = (pthread_t *) malloc(l_kl * sizeof(pthread_t));
    tab_klient_id = (int *) malloc(l_kl * sizeof(int));
    for(i = 0; i < l_kl; i++) {
        tab_klient_id[i] = i;
    }

    printf("\nOtwieramy pub!\n");
    printf("Liczba wolnych kufli: %d\n", l_kf);

    // Tworzenie wątków klientów
    for(i = 0; i < l_kl; i++) {
        pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
    }

    // Czekamy na zakończenie wszystkich wątków klientów
    for(i = 0; i < l_kl; i++) {
        pthread_join(tab_klient[i], NULL);
    }

    // Sprawdzenie końcowej liczby kufli
    if (l_kf < 0) {
        printf("BŁĄD: Liczba kufli na koniec jest niepoprawna!\n");
    } else {
        printf("Liczba kufli na koniec działania pubu jest zgodna: %d.\n", l_kf);
    }

    printf("\nZamykamy pub!\n");

    // Sprzątanie
    free(tab_klient);
    free(tab_klient_id);
    pthread_mutex_destroy(&kufle_mutex);
    return 0;
}

void * watek_klient(void * arg_wsk) {
    int moj_id = *((int *)arg_wsk);
    int i, sukces;
    int czeka_na_kufel = 0; // Zmienna do kontrolowania, czy klient czekał na kufel

    printf("Klient %d, wchodzi do pubu\n", moj_id);

    for(i = 0; i < ILE_MUSZE_WYPIC; i++) {
        sukces = 0;

        // Aktywne czekanie na dostępność kufla
        do {
            pthread_mutex_lock(&kufle_mutex);
            if (l_kf > 0) {
                l_kf--;  // Pobranie kufla
                sukces = 1;
                printf("Klient %d, pobrał kufel. Kufli pozostało: %d\n", moj_id, l_kf);
            } else {
                // Kufel niedostępny
                if (!czeka_na_kufel) {
                    printf("Klient %d, czeka na dostępny kufel\n", moj_id);
                    czeka_na_kufel = 1; // Zmieniamy status na czekający
                }
            }
            pthread_mutex_unlock(&kufle_mutex);

            if (sukces == 0) {
                // Kufel niedostępny, klient czeka
                usleep(100); // Czekanie na dostępność kufla
            } else {
                czeka_na_kufel = 0; // Klient nie czeka, resetujemy status
            }
        } while (sukces == 0);

        // Symulacja nalewania i picia
        printf("Klient %d, nalewa piwo\n", moj_id);
        usleep(300000); // Napełnianie kufla - 300ms
        printf("Klient %d, pije piwo\n", moj_id);
        nanosleep((struct timespec[]){{0, 50000000L}}, NULL); // Picie piwa - 50ms

        // Oddanie kufla
        pthread_mutex_lock(&kufle_mutex);
        l_kf++; // Zwracanie kufla
        printf("Klient %d, oddaje kufel. Kufli dostępnych: %d\n", moj_id, l_kf);
        pthread_mutex_unlock(&kufle_mutex);
    }

    printf("Klient %d, wychodzi z pubu\n", moj_id);
    return NULL;
}
