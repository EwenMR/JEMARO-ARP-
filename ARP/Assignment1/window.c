
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
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


int main(int argc, char* argv[]) {
    initscr();
    cbreak();
    

    int key;

    // fds to communicate with keyboardManager
    int window_keyboard[2];
    sscanf(argv[1], "%d %d", &window_keyboard[0],&window_keyboard[1]);
    close(window_keyboard[0]);

    // position[0],position[1]; position of the drone(x,y) of t_i-2
    // position[2],position[3]; position of the drone(x,y) of t_i-1
    // position[4],position[5]; position of the drone(x,y) now
    double position[6] = {COLS/2, LINES/2, COLS/2, LINES/2, COLS/2, LINES/2};
    // double position[6];
    int shared_seg_size = (1 * sizeof(position));

    // create semaphore ids
    sem_t * sem_id = sem_open(SEM_PATH, 0);

    // create shared memory pointer
    int shmfd  = shm_open(SHM_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    int first=0;
    while (1) {
        if(first==0){
            sem_wait(sem_id);
            memcpy(shm_ptr, position, shared_seg_size);
            sem_post(sem_id);
            first=1;
        }
        // refresh window
        WINDOW *win, *score;
        ncursesSetup(&win, &score);
        nodelay(win, TRUE);

        // move to the desired position and print "X", 
        mvwprintw(win, (int)position[5], (int)position[4], "X");
        wrefresh(win);

        // wait for user input
        key=wgetch(win);
        if (key != ERR) {
            // A key was pressed
            int ret= write(window_keyboard[1], &key, sizeof(key));
            if (ret<0){ 
                perror("writing error\n");
                close(window_keyboard[1]);
                exit(EXIT_FAILURE);
            }
            if((char)key==' '){
                close(window_keyboard[1]);
                exit(EXIT_SUCCESS);
            }
        }
        
        sem_wait(sem_id);
        // READ THE UPDATED X,Y POSITION FROM SHARED MEMORY
        memcpy(position,shm_ptr,shared_seg_size);
        sem_post(sem_id);
        clear();
    }

    close(window_keyboard[1]);
    shm_unlink(SHM_PATH);
    sem_close(sem_id);
    sem_unlink(SEM_PATH);

    endwin();

    return 0;
}