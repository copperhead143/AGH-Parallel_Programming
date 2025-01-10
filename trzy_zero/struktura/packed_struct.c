#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define NAME_LENGTH 20
#define MAX_BUFFER_SIZE 1000

typedef struct {
    char name[NAME_LENGTH];  // tablica znaków z imieniem
    int age;                 // liczba całkowita
    double height;           // liczba zmiennoprzecinkowa
} PersonData;

int main(int argc, char** argv) {
    int rank, size;
    char buffer[MAX_BUFFER_SIZE];
    int position = 0;

    PersonData send_data, recv_data;
    MPI_Status status;

    // Inicjalizacja MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Inicjalizacja danych przez proces 0
    if (rank == 0) {
        strcpy(send_data.name, "mścichuj");
        send_data.age = 22;
        send_data.height = 1.77;
    }

    // Zerowanie bufora i pozycji
    memset(buffer, 0, MAX_BUFFER_SIZE);
    position = 0;

    if (size > 1) {
        if (rank == 0) {
            // Pakowanie danych
            MPI_Pack(&send_data.name, NAME_LENGTH, MPI_CHAR, 
                     buffer, MAX_BUFFER_SIZE, &position, MPI_COMM_WORLD);
            MPI_Pack(&send_data.age, 1, MPI_INT, 
                     buffer, MAX_BUFFER_SIZE, &position, MPI_COMM_WORLD);
            MPI_Pack(&send_data.height, 1, MPI_DOUBLE, 
                     buffer, MAX_BUFFER_SIZE, &position, MPI_COMM_WORLD);

            // Wysłanie do następnego procesu
            MPI_Send(buffer, position, MPI_PACKED, 1, 0, MPI_COMM_WORLD);
        } else {
            // Odebranie bufora
            MPI_Recv(buffer, MAX_BUFFER_SIZE, MPI_PACKED, 
                     rank-1, 0, MPI_COMM_WORLD, &status);

            // Zerowanie pozycji przed rozpakowywaniem
            position = 0;

            // Rozpakowywanie danych
            MPI_Unpack(buffer, MAX_BUFFER_SIZE, &position, 
                       &recv_data.name, NAME_LENGTH, MPI_CHAR, MPI_COMM_WORLD);
            MPI_Unpack(buffer, MAX_BUFFER_SIZE, &position, 
                       &recv_data.age, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Unpack(buffer, MAX_BUFFER_SIZE, &position, 
                       &recv_data.height, 1, MPI_DOUBLE, MPI_COMM_WORLD);

            // Wypisanie otrzymanych danych
            printf("Proces %d odebrał: Imię= %s, Wiek= %d, Wzrost= %.2f\n", 
                   rank, recv_data.name, recv_data.age, recv_data.height);

            // Modyfikacja danych
            recv_data.age++;
            recv_data.height += 0.1;

            // Przygotowanie do wysłania dalej, jeśli nie ostatni proces
            if (rank < size - 1) {
                // Zerowanie bufora i pozycji
                memset(buffer, 0, MAX_BUFFER_SIZE);
                position = 0;

                // Ponowne pakowanie zmodyfikowanych danych
                MPI_Pack(&recv_data.name, NAME_LENGTH, MPI_CHAR, 
                         buffer, MAX_BUFFER_SIZE, &position, MPI_COMM_WORLD);
                MPI_Pack(&recv_data.age, 1, MPI_INT, 
                         buffer, MAX_BUFFER_SIZE, &position, MPI_COMM_WORLD);
                MPI_Pack(&recv_data.height, 1, MPI_DOUBLE, 
                         buffer, MAX_BUFFER_SIZE, &position, MPI_COMM_WORLD);

                // Wysłanie do następnego procesu
                MPI_Send(buffer, position, MPI_PACKED, rank+1, 0, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
    return 0;
}