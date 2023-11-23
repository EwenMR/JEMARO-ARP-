#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "include/constants.h"
#include "shared_memory.c"

int main() {
    

    // Shared memory segment for drone position
    initSemaphore();

    int shmid_pos = getSharedMemorySegment(KEY_POS, SHARED_DATA_SIZE);
    if (shmid_pos == -1) {
        // Handle the error, print a message, and decide what to do
        fprintf(stderr, "Failed to create/access shared memory.\n");
        // You might choose to exit or take another action
        exit(EXIT_FAILURE);
    }
    struct Position *drone_pos = attachSharedMemory(shmid_pos);

    // detachSharedMemory(drone_pos);
    initscr();
    // WINDOW *win = newwin(LINES - 10, COLS - 10, 5, 5);
    keypad(stdscr, TRUE);

    while (1) {
        // Update window with drone position
        clear();
        move(drone_pos->y, drone_pos->x);
        printw("X");
        

        // Wait for user input or some delay
        int key = getch();
        if (key != ERR) {
            // Handle user input if needed
        }

        // You can add more logic here based on your requirements

        refresh();  // Refresh the standard screen
    }

    // Detach shared memory
    

    // Clean up
    endwin();

    return 0;
}
