
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include "include/constants.h"
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>


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
    int initPos[4] = { // Start position of the window
        LINES/200,
        COLS/200,
        (LINES/200) + LINES - (LINES/5),
        COLS/200
    };

    *score = create_newwin(LINES*0.2, COLS*0.99, initPos[2], initPos[3]);
    *display = create_newwin(LINES*0.8, COLS*0.99, initPos[0], initPos[1]);
    
    // wrefresh(*display);
    // wrefresh(*score);
}


int main(int argc, char* argv[]) {
    initscr();
    
    int key;
    int first=0;

    // PIPES
    int window_keyboard[2]; // fds to communicate with keyboardManager
    sscanf(argv[1], "%d %d", &window_keyboard[0],&window_keyboard[1]);
    close(window_keyboard[0]);

    double position[6]={BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2}; //position of the drone (t-2,t-1,t)
    int shared_seg_size = (1 * sizeof(position));

    
    // SHARED MEMORY STUFF
    sem_t * sem_id = sem_open(SEM_PATH, 0); // create semaphore ids
    int shmfd  = shm_open(SHM_PATH, O_RDWR, S_IRWXU | S_IRWXG); // create shared memory pointer
    if (shmfd < 0){ 
        perror("shm_open");
        return -1;
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0); // map a shared memory segment into the address space
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* How data are handled
        -1 Initial drone position | window.c -> drone.c            | SHARED MEMORY
        -2 User input             | window.c -> keyboard_manager.c | PIPES
        -3 Command force          | keyboard_manager.c -> drone.c  | PIPES
        -4 Next drone position    | drone.c -> window.c            | SHARED MEMORY
    */

    while (1) {
        // refresh window
        WINDOW *win, *score;
        ncursesSetup(&win, &score);
        curs_set(0); // don't show cursor
        nodelay(win, TRUE);

        double scalex,scaley; // get the scale, to scale up the window to the desired size
        scalex=(double)BOARD_SIZE /((double)COLS*0.98);
        scaley=(double)BOARD_SIZE/((double)LINES*0.79);

        // gets the semaphore number. use it to check for deadlock
        int sem;
        sem_getvalue(sem_id,&sem);


        // print drone and score onto the window
        mvwprintw(win, (int)(position[5]/scaley), (int)(position[4]/scalex), "X");
        mvwprintw(score,1,20,"%f,%f", position[4],position[5]);
        wrefresh(win);
        wrefresh(score);
        
        // 1 
        // Send the initial drone position to drone.c via shared memory
        if(first==0){
            sem_wait(sem_id);
            memcpy(shm_ptr, position, shared_seg_size);
            sem_post(sem_id);
            first=1;
        }
        
        
        // 2
        key=wgetch(win); // wait for user input
        if (key != ERR) { // If a key was pressed properly
            int ret= write(window_keyboard[1], &key, sizeof(key)); //write to keyboard via pipe
            if (ret<0){ 
                perror("writing error\n");
                close(window_keyboard[1]);
                exit(EXIT_FAILURE);
            }
            if((char)key==' '){ //if space was pressed, close window
                close(window_keyboard[1]);
                exit(EXIT_SUCCESS);
            }
        }
        usleep(50000);


        // 4 Read from shared memory
        sem_wait(sem_id);
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