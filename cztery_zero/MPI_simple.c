#include <stdio.h>
#include <mpi.h>

#define MESSAGE_COUNT 5

int main(int argc, char** argv) {
    int rank, size;
    int message = 0;
    int prev, next;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ustal poprzednika i następcę
    prev = (rank - 1 + size) % size;
    next = (rank + 1) % size;

    // Inicjalizacja wiadomości przez proces 0
    if (rank == 0) {
        message = 42;  // Przykładowa wartość początkowa
        printf("Proces %d rozpoczyna sztafetę z wartością %d\n", rank, message);
        MPI_Send(&message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }

    // Wariant 1: Zamknięty pierścień
    while (1) {
        // Odbierz wiadomość od poprzednika
        MPI_Recv(&message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
        printf("Proces %d odebrał liczbę %d od procesu %d\n", rank, message, prev);

        // Zwiększ wartość
        message++;

        // Jeśli jesteśmy w procesie 0 i osiągnęliśmy limit, zakończ
        if (rank == 0 && message > MESSAGE_COUNT) break;

        // Wyślij do następnego procesu
        printf("Proces %d wysyła liczbę %d do procesu %d\n", rank, message, next);
        MPI_Send(&message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);

        // Zapobiegaj nieskończonej pętli dla procesu 0
        if (rank == 0) break;
    }

    MPI_Finalize();
    return 0;
}