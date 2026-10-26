#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define child 3

int pipes[child][2];
volatile sig_atomic_t interrupted = 0;

void interrupt_handler(int sig)
{
    interrupted = 1;
}

int main()
{
    char message[20];
    pid_t pid;

    // Create child processes
    for (int i = 0; i < child; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Pipe failed");
            return 1;
        }

        pid = fork();

        if (pid == 0) {
            char buffer[100] = {0};

            // Child ignores Ctrl+C
            signal(SIGINT, SIG_IGN);

            close(pipes[i][1]);

            // Close previous write ends
            for (int j = 0; j < i; j++) {
                close(pipes[j][1]);
            }

            ssize_t bytes_read = read(
                pipes[i][0], buffer, sizeof(buffer) - 1
            );

            if (bytes_read > 0) {
                printf("(Children) Child %d received: %s\n",
                       i + 1, buffer);
            }

            close(pipes[i][0]);
            exit(0);
        }

        if (pid < 0) {
            perror("Fork failed");
            return 1;
        }

        close(pipes[i][0]);
    }

    // Parent handles Ctrl+C
    signal(SIGINT, interrupt_handler);
    siginterrupt(SIGINT, 1);

    printf("\n(Parent) Enter a message: ");
    fflush(stdout);

    if (fgets(message, sizeof(message), stdin) != NULL
        && !interrupted) {

        message[strcspn(message, "\n")] = '\0';

        // Send message to children
        for (int i = 0; i < child; i++) {
            write(pipes[i][1], message, strlen(message) + 1);
            close(pipes[i][1]);
            pipes[i][1] = -1;
        }
    } else {
        // Handle Ctrl+C
        if (interrupted) {
            printf("\nCtrl+C initiated, program stopped.\n");
        }

        // Close remaining write ends
        for (int i = 0; i < child; i++) {
            if (pipes[i][1] != -1) {
                close(pipes[i][1]);
                pipes[i][1] = -1;
            }
        }
    }

    // Wait for all children
    for (int i = 0; i < child; i++) {
        wait(NULL);
    }

    printf("\n(Parent) All children have finished.\n");

    return 0;
}
