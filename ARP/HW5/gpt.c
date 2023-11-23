#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>

#define FILENAME "output.txt"

int main() {
    pid_t p1_pid, p2_pid;
    fd_set read_fds;
    int pipe_p1[2], pipe_p2[2];
    FILE *output_file;

    // Create pipes
    if (pipe(pipe_p1) == -1 || pipe(pipe_p2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create child process P1
    if ((p1_pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (p1_pid == 0) {
        // P1 process
        close(pipe_p1[0]); // Close unused read end

        char charToSend = 'A';
        while (1) {
            write(pipe_p1[1], &charToSend, sizeof(charToSend));
            usleep(100000); // 100,000 microseconds = 0.1 seconds
        }
    }

    // Create child process P2
    if ((p2_pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (p2_pid == 0) {
        // P2 process
        close(pipe_p2[0]); // Close unused read end

        char charToSend = 'B';
        while (1) {
            write(pipe_p2[1], &charToSend, sizeof(charToSend));
            usleep(200000); // 200,000 microseconds = 0.2 seconds
        }
    }

    // Parent process (P)
    close(pipe_p1[1]); // Close unused write end of P1's pipe
    close(pipe_p2[1]); // Close unused write end of P2's pipe

    // Open the output file for writing
    output_file = fopen(FILENAME, "w");
    if (output_file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(pipe_p1[0], &read_fds);
        FD_SET(pipe_p2[0], &read_fds);

        // Use select to wait for data to be available on either pipe
        if (select(pipe_p2[0] + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Check which pipe has data ready
        if (FD_ISSET(pipe_p1[0], &read_fds)) {
            char receivedChar;
            read(pipe_p1[0], &receivedChar, sizeof(receivedChar));
            fprintf(output_file, "%c", receivedChar);
        }

        if (FD_ISSET(pipe_p2[0], &read_fds)) {
            char receivedChar;
            read(pipe_p2[0], &receivedChar, sizeof(receivedChar));
            fprintf(output_file, "%c", receivedChar);
        }

        fflush(output_file); // Ensure the output is written immediately
    }

    // Close file and pipes
    fclose(output_file);
    close(pipe_p1[0]);
    close(pipe_p2[0]);

    return 0;
}
