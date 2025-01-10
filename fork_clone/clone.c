#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>

#define ROZMIAR_STOSU 1024*64

int funkcja_watku(void* argument) {
    printf("Child process (clone)\n");

    char *args[] = {"./print_info", NULL};
    execvp(args[0], args);
    perror("execvp");
    return 1;
}

int main() {
    void *stos;
    pid_t pid;
    
    
    stos = malloc(ROZMIAR_STOSU);
    if (stos == 0) {
        printf("Parent process - stack allocation error\n");
        exit(1);
    }

    pid = clone(&funkcja_watku, (void *)stos + ROZMIAR_STOSU,
                CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, NULL);

    if (pid == -1) {
        perror("clone");
        free(stos);
        exit(1);
    }

    waitpid(pid, NULL, __WCLONE);

    free(stos);
    return 0;
}