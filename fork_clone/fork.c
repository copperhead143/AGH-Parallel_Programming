#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pid, wynik;

    pid = fork();

    if (pid == 0) { // Child process
        printf("Child process (fork)\n");

        char *args[] = {"./print_info", NULL};
        wynik = execvp(args[0], args);
        if (wynik == -1) {
            perror("execvp");
            printf("Child process did not execute the program\n");
        }

        exit(0);
    } else if (pid > 0) { // Parent process
        wait(NULL);
    } else {
        perror("fork");
        exit(1);
    }

    return 0;
}