#include "headers/wrapfunc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int server_writer1[2];
int writer1_server[2];

int server_writer2[2];
int writer2_server[2];

int main(int argc, char *argv[]) {
    int stored = 0;
    int id = argv[1][0] - '0';
    printf("I am writer number %s\n", argv[1]);
    fflush(stdout);

    if (id == 1) {
        sscanf(argv[2], "%d %d|%d %d", &server_writer1[0], &server_writer1[1],
               &writer1_server[0], &writer1_server[1]);

        close(server_writer1[1]);
        close(writer1_server[0]);
    } else if (id == 2) {
        sscanf(argv[2], "%d %d|%d %d", &server_writer2[0], &server_writer2[1],
               &writer2_server[0], &writer2_server[1]);

        close(server_writer2[1]);
        close(writer2_server[0]);
    }
    srandom(id * 123);

    while (true) {
        int toSend = random() % 100;
        char stringToSend[20];
        char response[11];

        printf("Asking to write in cell %d\n", id - 1);
        fflush(stdout);

        if (id == 1) {
            m_write(writer1_server[1], "W0", 3);
            m_read(server_writer1[0], response, 10);

            if (response[0] == 'y') {
                sprintf(stringToSend, "%d", toSend);
                m_write(writer1_server[1], stringToSend,
                      strlen(stringToSend) + 1);
                printf("Writer 1 sent %d to server\n", toSend);
                fflush(stdout);

                stored++;
                printf("Writer 1 stored %d numbers\n", stored);
                fflush(stdout);
            }else{
                printf("Number has been rejected\n");
                fflush(stdout);
            }

        } else if (id == 2) {
            m_write(writer2_server[1], "W1", 3);
            m_read(server_writer2[0], response, 10);

            if (response[0] == 'y') {
                sprintf(stringToSend, "%d", toSend);
                m_write(writer2_server[1], stringToSend,
                      strlen(stringToSend) + 1);
                printf("Writer 2 sent %d to server\n", toSend);
                fflush(stdout);

                stored++;
                printf("Writer 2 stored %d numbers\n", stored);
                fflush(stdout);
            }else{
                printf("Number has been rejected\n");
                fflush(stdout);
            }
        }
        sleep(2);
    }

    return EXIT_SUCCESS;
}
