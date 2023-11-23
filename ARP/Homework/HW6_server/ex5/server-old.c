#include "headers/constants.h"
#include "headers/wrapfunc.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("I am Server\n");
    //ncerses stuff
    int cell[2] = {0, 0};
    // this is needed otherwise the condition cell[0] == cells[1] will never
    // occour
    int tmpCell[2] = {0, 0};
    int lastReadValue[] = {-1, -1};

    char receivedMsg[MAX_MSG_LEN];
    char toSend[MAX_MSG_LEN];

    int server_reader1[2];
    int reader1_server[2];

    int reader2_server[2];
    int server_reader2[2];

    int server_writer1[2];
    int writer1_server[2];

    int server_writer2[2];
    int writer2_server[2];

    fd_set master, reading;


    sscanf(argv[1], "%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d",
           &server_reader1[0], &server_reader1[1], &server_reader2[0],
           &server_reader2[1], &server_writer1[0], &server_writer1[1],
           &server_writer2[0], &server_writer2[1], &reader1_server[0],
           &reader1_server[1], &reader2_server[0], &reader2_server[1],
           &writer1_server[0], &writer1_server[1], &writer2_server[0],
           &writer2_server[1]);
    close(writer1_server[1]);
    close(writer2_server[1]);
    close(reader1_server[1]);
    close(reader2_server[1]);

    FD_ZERO(&master);
    FD_ZERO(&reading);

    FD_SET(writer1_server[0], &master);
    FD_SET(writer2_server[0], &master);
    FD_SET(reader1_server[0], &master);
    FD_SET(reader2_server[0], &master);

    int fds[4];
    fds[0] = writer1_server[0];
    fds[1] = writer2_server[0];
    fds[2] = reader1_server[0];
    fds[3] = reader2_server[0];

    int max_fd = -1;
    for(int i = 0; i < 4; i++){
        if(fds[i] > max_fd){
            max_fd = fds[i];
        }
    }
    while (1) {
        reading = master;
        int ret = m_select(max_fd + 1, &reading, NULL, NULL, NULL);
        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &reading)) {
                m_read(i, receivedMsg, MAX_MSG_LEN);
                // TODO error checking
                printf("Received %s ", receivedMsg);
                if (i == writer1_server[0]) {
                    printf("from writer 1\n");
                    if (cell[0] <= cell[1]) {
                        m_write(server_writer1[1], "y", 2);
                    } else {
                        m_write(server_writer1[1], "n", 2);
                    }

                    if (cell[0] <= cell[1]) {
                        printf("Cells: C0:%d C1:%d\n",tmpCell[0], tmpCell[1]);
                        m_read(i, receivedMsg, MAX_MSG_LEN);
                        sscanf(receivedMsg, "%d", &tmpCell[0]);
                    }

                } else if (i == writer2_server[0]) {
                    printf("from writer 2\n");

                    if (cell[0] >= cell[1]) {
                        m_write(server_writer2[1], "y", 2);
                    } else {
                        m_write(server_writer2[1], "n", 2);
                    }

                    if (cell[0] >= cell[1]) {
                        printf("Cells: C0:%d C1:%d\n",tmpCell[0], tmpCell[1]);
                        m_read(i, receivedMsg, MAX_MSG_LEN);
                        sscanf(receivedMsg, "%d", &tmpCell[1]);
                    }
                } else if (i == reader1_server[0]) {
                    printf("from reader 1\n");
                    // here tmpCell is used to have a more updated value
                    if (lastReadValue[0] != tmpCell[0]) {
                        lastReadValue[0] = tmpCell[0];
                        m_write(server_reader1[1], "y", 2);
                        sprintf(toSend, "%d", tmpCell[0]);
                        m_write(server_reader1[1],toSend, strlen(toSend)+1);
                    }else{
                        m_write(server_reader1[1],"n",2);
                    }
                } else if (i == reader2_server[0]) {
                    printf("from reader 2\n");
                    if (lastReadValue[1] != tmpCell[1]) {
                        lastReadValue[1] = tmpCell[1];
                        m_write(server_reader2[1], "y", 2);

                        sprintf(toSend, "%d", tmpCell[1]);
                        m_write(server_reader2[1],toSend, strlen(toSend)+1);
                    }else{
                        m_write(server_reader2[1],"n",2);
                    }
                }
            }
        }
        cell[0] = tmpCell[0];
        cell[1] = tmpCell[1];
    }
    return EXIT_SUCCESS;
}
