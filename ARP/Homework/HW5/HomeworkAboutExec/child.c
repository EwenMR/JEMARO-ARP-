#include "constants.h"
#include "headers/wrapfunc.h"
#include <ctype.h>
#include <curses.h>
#include <fcntl.h>
#include <math.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool isInside(MEVENT *mvt, int y, int x, int height, int width) {
    if (mvt->x >= x && mvt->x <= x + width && mvt->y >= y &&
        mvt->y <= y + height)
        return true;
    return false;
};

WINDOW *createStopWindow(int key, int height, int width, int starty,
                         int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    mvwprintw(local_win, 1, 1, "STOP");

    return local_win;
}

WINDOW *createMessageWindow(int height, int width, int starty, int startx,
                            float rate) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    mvwprintw(local_win, 1, 9, "Current sending rate: %f", rate);
    mvwprintw(local_win, 2, 8, "Type a number or . to insert rate");

    return local_win;
}

WINDOW *createInputWindow(int key, int height, int width, int starty,
                          int startx, char str[MAX_MESSAGE_LEN], int *idx,
                          bool *crPressed) {
    WINDOW *local_win;
    *crPressed = false;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    mvwprintw(local_win, 0, 0, "Input rate");

    if (key == KEY_ENTER || key == 10) {
        *crPressed = true;
    } else if (key == KEY_BACKSPACE || key == 127 || key == '\b') {
        if (*idx > 0) {
            str[--(*idx)] = '\0';
            mvwprintw(local_win, 1, 1, "%s", str);
        }
    } else {
        if (*idx < MAX_MESSAGE_LEN - 1) {
            if (isdigit(key) || key == '.') {
                str[(*idx)++] = key;
                str[(*idx)] = '\0';
                mvwprintw(local_win, 1, 1, "%s", str);
            }
        }
    }

    return local_win;
}

int main(int argc, char *argv[]) {
    int inputPipe[2];
    int readFromMaster, writeToMaster;
    char id[MAX_MESSAGE_LEN];

    sscanf(argv[1], "%s", id);
    sscanf(argv[2], "%d %d", &readFromMaster, &writeToMaster);

    close(readFromMaster);

    m_pipe(inputPipe);

    pid_t pid = m_fork();

    if (!pid) {
        char received[MAX_MESSAGE_LEN];
        float freq = 1.f / (id[0] - 'A' + 1);
        fd_set readset, master;
        struct timeval sleepTime;

        FD_ZERO(&master);
        FD_ZERO(&readset);
        FD_SET(inputPipe[0], &master);

        sleepTime.tv_sec = 0;
        sleepTime.tv_usec = 1 / freq * 1000000;
        char toSend[MAX_MESSAGE_LEN];
        sprintf(toSend, "%s", id);

        // child
        m_close(inputPipe[1]);
        while (1) {
            // m_write(writeToMaster, id, 2);
            m_write(writeToMaster, toSend, strlen(toSend) + 1);

            readset = master;
            m_select(inputPipe[0] + 1, &readset, NULL, NULL, &sleepTime);
            sleepTime.tv_sec = 0;
            sleepTime.tv_usec = 1 / freq * 1000000;
            for (int i = 0; i <= inputPipe[0]; i++) {
                if (FD_ISSET(i, &readset)) {
                    m_read(i, received, MAX_MESSAGE_LEN);
                    sscanf(received, "%f", &freq);

                    sleepTime.tv_usec = round(1.f / freq * 1000000);
                }
            }
        }
    } else {
        // father
        m_close(inputPipe[0]);
        WINDOW *inputWindow;
        WINDOW *messagesWindow;
        WINDOW *stopButton;
        int height = 3;
        int width = 30;
        int row, col;
        char string[MAX_MESSAGE_LEN];
        int idx = 0;
        float rate = 1;
        mmask_t *old;

        initscr();
        cbreak();
        noecho();

        // very important to receive mouse inputs
        keypad(stdscr, true);

        mousemask(ALL_MOUSE_EVENTS, old);
        printw("%s", argv[1]);

        int key = -1;
        while (1) {
            getmaxyx(stdscr, row, col);

            bool crPressed = false;

            inputWindow =
                createInputWindow(key, height, width, row / 2 - 1,
                                  (col - width) / 2, string, &idx, &crPressed);

            if (crPressed) {
                m_write(inputPipe[1], string, strlen(string) + 1);
                sscanf(string, "%f", &rate);
                strcpy(string, "");
                idx = 0;
            }
            messagesWindow =
                createMessageWindow(5, 50, row / 3, col / 2 - 25, rate);
            stopButton = createStopWindow(key, 3, 6, row / 5 * 4, col / 5 * 4);

            refresh();
            wrefresh(messagesWindow);
            wrefresh(inputWindow);
            wrefresh(stopButton);

            key = getch();

            MEVENT mvt;
            if (key == KEY_MOUSE) {
                if (getmouse(&mvt) == OK) {
                    if (mvt.bstate == BUTTON1_CLICKED &&
                        isInside(&mvt, row / 5 * 4, col / 5 * 4, 3, 6)) {
                        kill(pid, SIGKILL);
                        return EXIT_SUCCESS;
                    }
                }
            }

            wborder(inputWindow, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            wrefresh(inputWindow);
            delwin(inputWindow);
            delwin(messagesWindow);
            delwin(stopButton);

            // refresh();
            //  printf("Sent number %s\n", argv[1]);
        }
        getch();
        endwin();
    }
    return EXIT_SUCCESS;
}
