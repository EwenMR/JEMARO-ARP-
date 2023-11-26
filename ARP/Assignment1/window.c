
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "include/constants.h"
#include "shared_memory.c"
#define SHM_SIZE sizeof(struct Position)

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void ncursesSetup(WINDOW **display, WINDOW **score)
{
    int initPos[4] = {
        LINES/200,
        COLS/200,
        (LINES/200) + LINES - (LINES/5),
        COLS/200
    };

    *score = create_newwin(LINES / 5, COLS - (COLS/100), initPos[2], initPos[3]);
    *display = create_newwin(LINES - (LINES/5), COLS - (COLS/100), initPos[0], initPos[1]);
    
    wrefresh(*display);
    wrefresh(*score);
}

int window_keyboard[2];
int keyboard_window[2];

int main(int argc, char* argv[]) {
    initscr();
    cbreak();

    int pos_key[3];
    pos_key[0] = COLS/2;
    pos_key[1] = LINES/2;

    while (1) {
        WINDOW *win, *score;
        ncursesSetup(&win, &score);

        sscanf(argv[1], "%d %d|%d %d", &window_keyboard[0],&window_keyboard[1], &keyboard_window[0], &keyboard_window[1]);

        mvwprintw(win, pos_key[1], pos_key[0], "X");
        wrefresh(win);

        pos_key[2]=wgetch(win);

        int ret= write(window_keyboard[1], pos_key, sizeof(pos_key));
        if (ret<0){ 
            perror("writing error\n");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }


        int ret2 = read(keyboard_window[0], pos_key, sizeof(pos_key));
        if (ret2<0){
            perror("reading error\n");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        clear();
    }

    endwin();

    return 0;
}