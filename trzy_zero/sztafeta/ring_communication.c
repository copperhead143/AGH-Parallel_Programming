#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MESSAGE_TAG 0

int main(int argc, char** argv) {
    int rank, size;
    int data = 42;  // Początkowa wartość
    int received_data;
    
    int prev, next;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ustalenie poprzednika i następcy
    prev = (rank - 1 + size) % size;
    next = (rank + 1) % size;

    // Wariant 1: Zamknięty pierścień
    if (rank == 0) {
        // Proces startowy wysyła początkową wartość
        printf("Proces %d inicjuje pierścień z wartością %d\n", rank, data);
        MPI_Send(&data, 1, MPI_INT, next, MESSAGE_TAG, MPI_COMM_WORLD);
    }

    // Każdy proces odbiera i wysyła dalej
    MPI_Recv(&received_data, 1, MPI_INT, prev, MESSAGE_TAG, MPI_COMM_WORLD, &status);
    
    // Modyfikacja danych
    received_data++;
    
    printf("Proces %d odebrał liczbę %d od procesu %d\n", rank, received_data, prev);
    
    // Wysłanie do następnego procesu, chyba że jesteśmy w ostatnim
    if (rank != size - 1) {
        printf("Proces %d wysyła liczbę %d do procesu %d\n", rank, received_data, next);
        MPI_Send(&received_data, 1, MPI_INT, next, MESSAGE_TAG, MPI_COMM_WORLD);
    } else {
        // Ostatni proces może wysłać z powrotem do pierwszego (opcjonalnie)
        printf("Proces %d wysyła liczbę %d do procesu 0, zamykając pierścień\n", rank, received_data);
        MPI_Send(&received_data, 1, MPI_INT, 0, MESSAGE_TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}