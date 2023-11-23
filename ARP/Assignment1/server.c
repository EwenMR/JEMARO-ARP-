//server.c
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "include/constants.h"
#include "shared_memory.c"


WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void ncursesSetup(WINDOW **display, WINDOW **logger)
{
    int initPos[2] = {
        LINES/200,
        COLS/200
    };

    *display = create_newwin(LINES - (LINES/100), COLS - (COLS/100), initPos[0], initPos[1]);
    
    *logger = create_newwin(LINES / 2, COLS / 2, initPos[0], initPos[1]);

    wrefresh(*display);
}


int main(int argc, char *argv[])
{
    int key;
    initscr();

    // cbreak();
    // WINDOW *display, *logger;
    // ncursesSetup(&display, &logger);
    
   


    // // Shared memory segments for drone position, goals, and obstacles
    int shmid_pos = getSharedMemorySegment(KEY_POS, sizeof(struct Position));
    
    // // Attach shared memory segments to the process
    struct Position *drone_pos = shmat(shmid_pos, NULL, 0);





    // while(1){
    //     move(drone_pos->y,drone_pos->x);
    //     printw("X");
    //     refresh();
    //     drone_pos->key = getch();
    //     // clear();
    // }



    detachSharedMemory(drone_pos);
    endwin();
    return 0;
}
