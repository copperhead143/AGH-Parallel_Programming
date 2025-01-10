#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h> // Nagłówek potrzebny do użycia funkcji clone

int zmienna_globalna = 0; // Zmienna globalna, która będzie zwiększana przez wątki
#define ROZMIAR_STOSU 1024*64 // Definicja rozmiaru stosu dla wątków

// Funkcja wątku, która będzie wykonywana przez wątki
int funkcja_watku(void* argument) {
    int* local = (int*)argument; // Rzutowanie argumentu na wskaźnik do zmiennej lokalnej
    for (int i = 0; i < 100000; i++) { // Pętla wykonująca się 100000 razy
        (*local)++; // Zwiększanie zmiennej lokalnej
        zmienna_globalna++; // Zwiększanie zmiennej globalnej
    }
    // Wypisanie wartości zmiennych po zakończeniu pętli
    printf("Thread finished: Local variable: %d, Global variable: %d\n", *local, zmienna_globalna);
    return 0; // Zakończenie funkcji wątku
}

int main() {
    void *stos, *stos1; // Wskaźniki do stosów dla dwóch wątków
    pid_t pid, pid1; // Identyfikatory procesów dla dwóch wątków
    int local_variable1 = 0; // Zmienna lokalna dla pierwszego wątku
    int local_variable2 = 0; // Zmienna lokalna dla drugiego wątku

    // Alokacja pamięci dla stosów
    stos = malloc(ROZMIAR_STOSU);
    stos1 = malloc(ROZMIAR_STOSU);
    if (stos == 0 || stos1 == 0) { // Sprawdzenie, czy alokacja pamięci się powiodła
        printf("Proces nadrzędny - blad alokacji stosu\n");
        exit(1); // Zakończenie programu w przypadku błędu alokacji
    }

    // Utworzenie pierwszego wątku za pomocą funkcji clone
    pid = clone(&funkcja_watku, (void *)stos + ROZMIAR_STOSU,
                CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &local_variable1);

    // Utworzenie drugiego wątku za pomocą funkcji clone
    pid1 = clone(&funkcja_watku, (void *)stos1 + ROZMIAR_STOSU,
                 CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &local_variable2);

    // Oczekiwanie na zakończenie pierwszego wątku
    waitpid(pid, NULL, __WCLONE);
    // Oczekiwanie na zakończenie drugiego wątku
    waitpid(pid1, NULL, __WCLONE);

    // Wypisanie wartości zmiennych po zakończeniu pracy wątków
    printf("Main: Local variable 1: %d\n", local_variable1);
    printf("Main: Local variable 2: %d\n", local_variable2);
    printf("Main: Global variable: %d\n", zmienna_globalna);

    // Zwolnienie pamięci zajmowanej przez stosy
    free(stos);
    free(stos1);

    return 0; // Zakończenie programu
}