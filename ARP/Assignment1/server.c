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



int main(int argc, char *argv[])
{
    int key;
    initscr();

    // cbreak();
    // WINDOW *display, *logger;
    // ncursesSetup(&display, &logger);
    
   


    // // Shared memory segments for drone position, goals, and obstacles
    // int shmid_pos = getSharedMemorySegment(KEY_POS, sizeof(struct Position));
    
    // // Attach shared memory segments to the process
    // struct Position *drone_pos = shmat(shmid_pos, NULL, 0);





    // while(1){
    //     move(drone_pos->y,drone_pos->x);
    //     printw("X");
    //     refresh();
    //     drone_pos->key = getch();
    //     // clear();
    // }



    // detachSharedMemory(drone_pos);
    endwin();
    return 0;
}
