#include <ncurses.h>
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include "include/constants.h"
#include "shared_memory.c"

int main() {
    // Initialize ncurses
    initscr();
    // Don't display cursor
    curs_set(0);

    int position[6];
    int shared_seg_size = (1 * sizeof(position));

    sem_t * sem_id = sem_open(SEM_PATH, 0);

    int shmfd  = shm_open(SHM_PATH, O_RDONLY, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);


    while (1) {

        sem_wait(sem_id);
        // copy memory to local cells
        memcpy(position, shm_ptr, shared_seg_size);
        // post semaphore as soon as we are done
        sem_post(sem_id);

        int row, col;
        getmaxyx(stdscr, row, col);
        mvprintw(row / 2, (col - 5) / 2, "hello %d %d %d",position[0],position[1],position[2]);

        // Refresh the screen
        refresh();
    }

    
    shm_unlink(SHM_PATH);
    sem_close(sem_id);
    sem_unlink(SEM_PATH);


    // Print "hello" in the center of the screen
    

    // End ncurses
    endwin();

    return 0;
}
