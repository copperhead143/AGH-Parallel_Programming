#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

#define MAX_TEXT 100
#define MAX_STRUCTURES 10

typedef struct {
    char text[MAX_TEXT];
    int text_length;
    int vowel_count;
    int uppercase_count;
} TextProcessingData;

// Funkcje przetwarzania dla różnych procesów
void process_text_count_length(TextProcessingData* data) {
    data->text_length = strlen(data->text);
}

void process_text_count_vowels(TextProcessingData* data) {
    data->vowel_count = 0;
    for (int i = 0; data->text[i] != '\0'; i++) {
        char ch = tolower(data->text[i]);
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u') {
            data->vowel_count++;
        }
    }
}

void process_text_to_uppercase(TextProcessingData* data) {
    data->uppercase_count = 0;
    for (int i = 0; data->text[i] != '\0'; i++) {
        data->text[i] = toupper(data->text[i]);
        if (isupper(data->text[i])) data->uppercase_count++;
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Datatype text_type;
    TextProcessingData structures[MAX_STRUCTURES];
    int structure_count;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Definicja struktury MPI
    int blocklengths[3] = {MAX_TEXT, 1, 1};
    MPI_Aint displacements[3];
    MPI_Datatype types[3] = {MPI_CHAR, MPI_INT, MPI_INT};

    MPI_Aint base_address;
    MPI_Get_address(&structures[0], &base_address);
    MPI_Get_address(&structures[0].text, &displacements[0]);
    MPI_Get_address(&structures[0].text_length, &displacements[1]);
    MPI_Get_address(&structures[0].vowel_count, &displacements[2]);

    displacements[0] -= base_address;
    displacements[1] -= base_address;
    displacements[2] -= base_address;

    MPI_Type_create_struct(3, blocklengths, displacements, types, &text_type);
    MPI_Type_commit(&text_type);

    // Proces 0 inicjalizuje struktury
    if (rank == 0) {
        structure_count = 3;
        strcpy(structures[0].text, "hello");
        strcpy(structures[1].text, "world");
        strcpy(structures[2].text, "mpi");
    }

    // Zaawansowane przetwarzanie potokowe
    if (rank == 0) {
        // Wysyłanie struktur do następnego procesu
        MPI_Send(&structure_count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(structures, structure_count, text_type, 1, 1, MPI_COMM_WORLD);
    } else {
        // Odbieranie struktur
        MPI_Recv(&structure_count, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(structures, structure_count, text_type, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Specyficzne przetwarzanie dla każdego procesu
        switch(rank) {
            case 1:
                for (int i = 0; i < structure_count; i++) 
                    process_text_count_length(&structures[i]);
                break;
            case 2:
                for (int i = 0; i < structure_count; i++) 
                    process_text_count_vowels(&structures[i]);
                break;
            case 3:
                for (int i = 0; i < structure_count; i++) 
                    process_text_to_uppercase(&structures[i]);
                break;
        }

        // Wyświetlenie wyników
        for (int i = 0; i < structure_count; i++) {
            printf("Proces %d: Tekst=%s, Długość=%d, Samogłoski=%d, Wielkie litery=%d\n", 
                   rank, structures[i].text, 
                   structures[i].text_length, 
                   structures[i].vowel_count, 
                   structures[i].uppercase_count);
        }

        // Wysłanie dalej, jeśli nie ostatni proces
        if (rank < size - 1) {
            MPI_Send(&structure_count, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
            MPI_Send(structures, structure_count, text_type, rank+1, 1, MPI_COMM_WORLD);
        }
    }

    MPI_Type_free(&text_type);
    MPI_Finalize();
    return 0;
}