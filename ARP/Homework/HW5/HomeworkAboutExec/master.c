#include "constants.h"
#include "wrapfunc.h"
#include <bits/types/struct_timeval.h>
#include <fcntl.h>
#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

int retrieveFd(int pid, int arr[CHILDS_NUM * 2]) {
    for (int i = 0; i < CHILDS_NUM; i++) {
        if (arr[i * 2] == pid)
            return arr[i * 2 + 1];
    }
    return -1;
}

void summon(char **programArgs) {
    execvp("konsole", programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    char *argsChild[] = {"konsole", "-e", "./child", "p", "p", NULL};
    int max_fd = -1;
    int pipes[2];
    int childs = CHILDS_NUM;
    int toClose[CHILDS_NUM];
    int countA, countB;

    countA = countB = 0;

    // tis array is used to save the association between a file descriptor
    // and the child at witch is sent
    // the structure is that if i is the child pid than i+1 is the file
    // descriptor of the pipe that it uses to send data
    int pid_fd[CHILDS_NUM * 2];

    fd_set master, reader;
    FD_ZERO(&master);
    FD_ZERO(&reader);

    for (int i = 0; i < CHILDS_NUM; i++) {
        m_pipe(pipes);

        FD_SET(pipes[0], &master);

        if (max_fd < pipes[0])
            max_fd = pipes[0];

        pid_t pid = m_fork();

        if (!pid) {
            char str[MAX_MESSAGE_LEN];
            char str2[MAX_MESSAGE_LEN];
            sprintf(str, "%c", 'A' + i);
            argsChild[3] = str;
            sprintf(str2, "%d %d", pipes[0], pipes[1]);
            argsChild[4] = str2;
            summon(argsChild);
        } else {
            m_close(pipes[1]);
            printf("Summoned child with pid %d\n", pid);
            fflush(stdout);
            toClose[i] = pipes[0];

            pid_fd[i * 2] = pid;
            pid_fd[i * 2 + 1] = pipes[0];
        }
    }

    char received_msg[100];

    int stat;

    while (1) {
        int pidc = waitpid(-1, &stat, WNOHANG);
        if (pidc > 0) {
            int retrieved = retrieveFd(pidc, pid_fd);
            if (retrieved > 0) {
                FD_CLR(retrieved, &master);
            }
            printf("Child %d terminated with status %d\n", pidc, stat);
            fflush(stdout);
        }
        reader = master;
        m_select(max_fd + 1, &reader, NULL, NULL, NULL);
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &reader)) {
                int ret = m_read(i, received_msg, MAX_MESSAGE_LEN);
                if (ret == 0) {
                    FD_CLR(i, &master);
                    close(i);
                    printf("Closed pipe %d\n", i);
                    fflush(stdout);
                    if (!(--childs)) {
                        printf("Father terminating\n");
                        fflush(stdout);

                        printf("A has been received %d times and B has been received %d times\n", countA, countB);
                        fflush(stdout);
                        return EXIT_SUCCESS;
                    }
                    continue;
                }
                printf("%s\n", received_msg);

                if (!strcmp(received_msg, "A"))
                    countA++;
                else if (!strcmp(received_msg, "B"))
                    countB++;

                int fd = m_open("log.txt", O_WRONLY | O_APPEND);
                strcat(received_msg, "\n");
                m_write(fd, received_msg, strlen(received_msg));
                m_close(fd);
                fflush(stdout);
            }
        }
    }

    return EXIT_FAILURE;
}
