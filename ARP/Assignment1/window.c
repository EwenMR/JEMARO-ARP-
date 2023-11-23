#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "include/constants.h"
#include "shared_memory.c"

int main() {
    initscr();

    WINDOW *win = newwin(LINES - 10, COLS - 10, 5, 5);
    box(win, 0, 0);
    wrefresh(win);
    


    // Shared memory segment for drone position
    initSemaphore();

    int shmid_pos = getSharedMemorySegment(KEY_POS, sizeof(struct Position));
    struct Position *drone_pos = attachSharedMemory(shmid_pos);
        // // Initialize positions
    drone_pos->x = COLS/2;
    drone_pos->y = LINES/2;


    while (1) {
        // Update window with drone position
        
        move(drone_pos->y, drone_pos->x);
        printw("X");
        drone_pos->key = getch();
        refresh();
        clear();
    }

    // Detach shared memory
    detachSharedMemory(drone_pos);

    // Clean up
    endwin();

    return 0;
}
