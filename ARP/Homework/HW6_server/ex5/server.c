#include "headers/constants.h"
#include "headers/wrapfunc.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <ncurses.h>

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void ncursesSetup(WINDOW **topLeft,WINDOW **topRight,WINDOW **bottomLeft,WINDOW **bottomRight,WINDOW **logger){
    const int cellDim = 6;
    int initPos[10] = {
        (LINES/2 - cellDim)/2,
        COLS/2 - cellDim,
        (LINES/2 - cellDim)/2,
        COLS/2,
        LINES/4,
        COLS/2 - cellDim,
        LINES/4,
        COLS/2,
        LINES/2,
        COLS/4
    };
    *topLeft = create_newwin(cellDim/2, cellDim, initPos[0], initPos[1]);
    *topRight = create_newwin(cellDim/2, cellDim, initPos[2], initPos[3]);
    *bottomLeft = create_newwin(cellDim/2, cellDim, initPos[4], initPos[5]);
    *bottomRight = create_newwin(cellDim/2, cellDim, initPos[6], initPos[7]);
    *logger = create_newwin(LINES/2, COLS/2, initPos[8], initPos[9]);

    mvwprintw(*topLeft,1,2,"C0");
    wrefresh(*topLeft);
    mvwprintw(*topRight,1,2,"C1");
    wrefresh(*topRight);
}

void loggerAdd(int* loggerDim, WINDOW** logger, char loggerStrings[MAX_LOGGER_LEN][MAX_MSG_LEN], const char string[MAX_MSG_LEN]){
    if(*loggerDim < MAX_LOGGER_LEN)(*loggerDim)++;
    for(int i = *loggerDim; i >= 0; i--){
       //dest - src
       strcpy(loggerStrings[i+1], loggerStrings[i]);
    }

    strcpy(loggerStrings[0], string);

    for(int i = 0; i < *loggerDim && i < getmaxy(*logger)-2; i++){
        mvwprintw(*logger, i+1, (getmaxx(*logger)-strlen(string))/2, "%s", loggerStrings[i]);
    }
    wrefresh(*logger);
}

int main(int argc, char *argv[]) {
    //ncerses stuff
    char loggerStrings[MAX_LOGGER_LEN][MAX_MSG_LEN];
    char logmsg[MAX_MSG_LEN+30];// it needs to accomodate also strings like "RECEIVED" 
    int loggerDim = 0;
    initscr();
    cbreak();

    WINDOW *topLeft, *topRight, *bottomLeft, *bottomRight, *logger;
    ncursesSetup(&topLeft, &topRight, &bottomLeft, &bottomRight, &logger);

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
    int runs = 0;
    while (1) {
        runs++;
        reading = master;
        int ret = m_select(max_fd + 1, &reading, NULL, NULL, NULL);


        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &reading)) {
                m_read(i, receivedMsg, MAX_MSG_LEN);
                // TODO error checking
                if (i == writer1_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from writer 1", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);
                    if (cell[0] <= cell[1]) {
                        m_write(server_writer1[1], "y", 2);
                    } else {
                        m_write(server_writer1[1], "n", 2);
                    }

                    if (cell[0] <= cell[1]) {
                        m_read(i, receivedMsg, MAX_MSG_LEN);
                        sscanf(receivedMsg, "%d", &tmpCell[0]);

                        mvwprintw(bottomLeft, 1, 1, "%d", tmpCell[0]);
                        mvwprintw(bottomRight, 1, 1, "%d", tmpCell[1]);
                        wrefresh(bottomLeft);
                        wrefresh(bottomRight);
                    }

                } else if (i == writer2_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from writer 2", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);

                    if (cell[0] >= cell[1]) {
                        m_write(server_writer2[1], "y", 2);
                    } else {
                        m_write(server_writer2[1], "n", 2);
                    }

                    if (cell[0] >= cell[1]) {
                        m_read(i, receivedMsg, MAX_MSG_LEN);
                        sscanf(receivedMsg, "%d", &tmpCell[1]);

                        mvwprintw(bottomLeft, 1, 1, "%d", tmpCell[0]);
                        mvwprintw(bottomRight, 1, 1, "%d", tmpCell[1]);
                        wrefresh(bottomLeft);
                        wrefresh(bottomRight);
                    }
                } else if (i == reader1_server[0]) {
                    // here tmpCell is used to have a more updated value
                    sprintf(logmsg, "RECEIVED %s from reader 1", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);

                    if (lastReadValue[0] != tmpCell[0]) {
                        lastReadValue[0] = tmpCell[0];
                        m_write(server_reader1[1], "y", 2);
                        sprintf(toSend, "%d", tmpCell[0]);
                        m_write(server_reader1[1],toSend, strlen(toSend)+1);
                    }else{
                        m_write(server_reader1[1],"n",2);
                    }
                } else if (i == reader2_server[0]) {
                    sprintf(logmsg, "RECEIVED %s from reader 2", receivedMsg);
                    loggerAdd(&loggerDim, &logger, loggerStrings, logmsg);
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
    endwin();
    return EXIT_SUCCESS;
}
