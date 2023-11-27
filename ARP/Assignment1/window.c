
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include "include/constants.h"
#include "shared_memory.c"
#include <math.h>
#define BOARD_SIZE 100

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

    *score = create_newwin(LINES*0.2, COLS*0.99, initPos[2], initPos[3]);
    *display = create_newwin(LINES*0.8, COLS*0.99, initPos[0], initPos[1]);
    
    wrefresh(*display);
    wrefresh(*score);
}


int main(int argc, char* argv[]) {
    initscr();
    // cbreak();
    double scalex,scaley;

    // get the scale, to scale up the window to the desired size
    scalex=(double)BOARD_SIZE /((double)COLS*0.99);
    scaley=(double)BOARD_SIZE/((double)LINES*0.80);

    

    int key;

    // fds to communicate with keyboardManager
    int window_keyboard[2];
    sscanf(argv[1], "%d %d", &window_keyboard[0],&window_keyboard[1]);
    close(window_keyboard[0]);

    // position[0],position[1]; position of the drone(x,y) of t_i-2
    // position[2],position[3]; position of the drone(x,y) of t_i-1
    // position[4],position[5]; position of the drone(x,y) now
    // double position[6] = {scalex*COLS/2, scaley*LINES/2, scalex*COLS/2, scaley*LINES/2, scalex*COLS/2, scaley*LINES/2};
    double position[6]={BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2};
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
        // refresh window
        WINDOW *win, *score;
        ncursesSetup(&win, &score);
        curs_set(0);
        nodelay(win, TRUE);
        wprintw(win,"%d,%d,%f,%f,%f,%f",COLS,LINES,scalex,scaley,position[5],position[5]/scaley);

        // move to the desired position and print "X", 
        int sem;
        sem_getvalue(sem_id,&sem);
        mvwprintw(win, (int)(position[5]/scaley), (int)(position[4]/scalex), "X %d", sem);
        // mvwprintw(win, 5, 5, "X");
        wrefresh(win);
        
        if(first==0){
            sem_wait(sem_id);
            memcpy(shm_ptr, position, shared_seg_size);
            sem_post(sem_id);
            first=1;
        }
        
        

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
        usleep(50000);
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