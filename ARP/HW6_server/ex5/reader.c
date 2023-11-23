#include "headers/constants.h"
#include "headers/wrapfunc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int server_reader1[2];
int reader1_server[2];

int server_reader2[2];
int reader2_server[2];

int main(int argc, char *argv[]) {
    char response[MAX_MSG_LEN];
    int intResponse;
    char toSend[MAX_MSG_LEN];
    int id = argv[1][0] - '0';

    if (id == 1) {
        sscanf(argv[2], "%d %d|%d %d", &server_reader1[0], &server_reader1[1],
               &reader1_server[0], &reader1_server[1]);
        close(server_reader1[1]);
        close(reader1_server[0]);
    } else if (id == 2) {
        sscanf(argv[2], "%d %d|%d %d", &server_reader2[0], &server_reader2[1],
               &reader2_server[0], &reader2_server[1]);
        close(server_reader2[1]);
        close(reader2_server[0]);
    }

    printf("I am reader number %s\n", argv[1]);
    fflush(stdout);

    while (true) {
        if (id == 1) {
            m_write(reader1_server[1], "R0", 3);
            printf("Sent reading request\n");
            m_read(server_reader1[0], response, 2);
            if (response[0] == 'y') {
                printf("Read accepted\n");
                m_read(server_reader1[0], response, 10);
                sscanf(response, "%d", &intResponse);
                printf("Cell 0 contains %d\n", intResponse);
                fflush(stdout);

                int file = m_open("logfiles/log_r.log", O_WRONLY|O_APPEND);
                sprintf(response, "Reader 1 read: %d\n", intResponse);
                m_write(file, response, strlen(response)+1);
                close(file);
            } else {
                printf("Read rejected\n");
                fflush(stdout);
            }
        } else {
            m_write(reader2_server[1], "R1", 3);
            printf("Sent reading request\n");
            m_read(server_reader2[0], response, 2);
            if (response[0] == 'y') {
                printf("Read accepted\n");
                m_read(server_reader2[0], response, 10);
                sscanf(response, "%d", &intResponse);
                printf("Cell 1 contains %d\n", intResponse);
                fflush(stdout);

                int file = m_open("logfiles/log_r.log", O_WRONLY|O_APPEND);
                sprintf(response, "Reader 2 read: %d\n", intResponse);
                m_write(file, response, strlen(response)+1);
                close(file);
                
            } else {
                printf("Read rejected\n");
                fflush(stdout);
            }
        }

        sleep(3);
    }
    return EXIT_SUCCESS;
}
