#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipes1[2], pipes2[2];
    pid_t pid1, pid2, pid3;

    // Create the first pipe
    if (pipe(pipes1) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    // Create the second pipe
    if (pipe(pipes2) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    // Fork the first child process
    pid1 = fork();
    if (pid1 == -1) {
        perror("Fork failed");
        return 1;
    }

    if (pid1 == 0) {
        // First child process
        dup2(pipes1[1], STDOUT_FILENO);
        close(pipes1[0]);
        close(pipes1[1]);
        close(pipes2[0]);
        close(pipes2[1]);
        execlp("ls", "ls", "-ltr", NULL);
        exit(1);
    } else {
        // Parent process
        pid2 = fork();
        if (pid2 == -1) {
            perror("Fork failed");
            return 1;
        }

        if (pid2 == 0) {
            // Second child process
            dup2(pipes1[0], STDIN_FILENO);
            dup2(pipes2[1], STDOUT_FILENO);
            close(pipes1[0]);
            close(pipes1[1]);
            close(pipes2[0]);
            close(pipes2[1]);
            execlp("grep", "grep", "3376", NULL);
            exit(1);
        } else {
            // Parent process
            pid3 = fork();
            if (pid3 == -1) {
                perror("Fork failed");
                return 1;
            }

            if (pid3 == 0) {
                // Third child process
                dup2(pipes2[0], STDIN_FILENO);
                close(pipes1[0]);
                close(pipes1[1]);
                close(pipes2[0]);
                close(pipes2[1]);
                execlp("wc", "wc", "-l", NULL);
                exit(1);
            } else {
                // Parent process
                close(pipes1[0]);
                close(pipes1[1]);
                close(pipes2[0]);
                close(pipes2[1]);
                int status1, status2, status3;
                waitpid(pid1, &status1, 0);
                waitpid(pid2, &status2, 0);
                waitpid(pid3, &status3, 0);
            }
        }
    }

    return 0;
}