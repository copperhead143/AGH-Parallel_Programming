#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define NAME_LENGTH 50
#define MAX_DESCRIPTION 100

typedef struct {
    char name[NAME_LENGTH];        // imię
    int age;                       // wiek
    double salary;                 // pensja
    char job_description[MAX_DESCRIPTION];  // opis pracy
} EmployeeData;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Datatype employee_type;
    EmployeeData send_data, recv_data;

    // Inicjalizacja MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Definicje dla MPI_Type_create_struct
    int blocklengths[4] = {NAME_LENGTH, 1, 1, MAX_DESCRIPTION};
    MPI_Aint displacements[4];
    MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_DOUBLE, MPI_CHAR};

    // Obliczenie przesunięć
    MPI_Aint base_address;
    MPI_Get_address(&send_data, &base_address);
    MPI_Get_address(&send_data.name, &displacements[0]);
    MPI_Get_address(&send_data.age, &displacements[1]);
    MPI_Get_address(&send_data.salary, &displacements[2]);
    MPI_Get_address(&send_data.job_description, &displacements[3]);

    // Korekta przesunięć
    displacements[0] -= base_address;
    displacements[1] -= base_address;
    displacements[2] -= base_address;
    displacements[3] -= base_address;

    // Utworzenie nowego typu danych
    MPI_Type_create_struct(4, blocklengths, displacements, types, &employee_type);
    MPI_Type_commit(&employee_type);

    if (rank == 0) {
        // Inicjalizacja danych
        strcpy(send_data.name, "Kowalski");
        send_data.age = 35;
        send_data.salary = 5000.50;
        strcpy(send_data.job_description, "Programista C/MPI");
    }

    if (size > 1) {
        if (rank == 0) {
            // Wysłanie struktury do następnego procesu
            MPI_Send(&send_data, 1, employee_type, 1, 0, MPI_COMM_WORLD);
        } else {
            // Odebranie struktury
            MPI_Recv(&recv_data, 1, employee_type, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Wyświetlenie i modyfikacja danych
            printf("Proces %d odebrał dane: %s, lat %d, pensja %.2f, praca: %s\n", 
                   rank, recv_data.name, recv_data.age, 
                   recv_data.salary, recv_data.job_description);

            // Modyfikacja danych
            recv_data.age++;
            recv_data.salary *= 1.1;

            // Wysłanie dalej, jeśli nie ostatni proces
            if (rank < size - 1) {
                MPI_Send(&recv_data, 1, employee_type, rank+1, 0, MPI_COMM_WORLD);
            }
        }
    }

    // Zwolnienie utworzonego typu
    MPI_Type_free(&employee_type);
    MPI_Finalize();
    return 0;
}