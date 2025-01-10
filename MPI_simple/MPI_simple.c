#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>  // dla gethostname()
#include <string.h>  // dla strlen()

#include "mpi.h"

#define MAX_HOSTNAME 256

int main(int argc, char** argv) {
    int rank, size;
    char hostname[MAX_HOSTNAME];
    char recv_hostname[MAX_HOSTNAME];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    gethostname(hostname, MAX_HOSTNAME);

    if (size > 1) {
        if (rank != 0) {
            // Wysyłanie rangi i nazwy hosta do procesu 0
            MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(hostname, strlen(hostname) + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
        } else {
            for (int i = 1; i < size; i++) {
                int recv_rank;
                MPI_Recv(&recv_rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(recv_hostname, MAX_HOSTNAME, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
                
                printf("Proces %d z hosta %s wysłał wiadomość\n", recv_rank, recv_hostname);
            }
        }
    } else {
        printf("Pojedynczy proces o randze: %d na hoście %s\n", rank, hostname);
    }

    MPI_Finalize();
    return 0;
}