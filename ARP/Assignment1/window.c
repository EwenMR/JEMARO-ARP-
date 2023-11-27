
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

<<<<<<< HEAD

=======
>>>>>>> master
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
<<<<<<< HEAD
=======


>>>>>>> master
    int initPos[4] = {
        LINES/200,
        COLS/200,
        (LINES/200) + LINES - (LINES/5),
        COLS/200
    };

<<<<<<< HEAD
    *score = create_newwin(LINES / 5, COLS - (COLS/100), initPos[2], initPos[3]);
    *display = create_newwin(LINES - (LINES/5), COLS - (COLS/100), initPos[0], initPos[1]);
    
    // mvwprintw(*display,1,2,"O");
    // wrefresh(*display);
=======
    *score = create_newwin(LINES*0.2, COLS*0.99, initPos[2], initPos[3]);
    *display = create_newwin(LINES*0.8, COLS*0.99, initPos[0], initPos[1]);
    
>>>>>>> master
    wrefresh(*display);
    wrefresh(*score);
}

<<<<<<< HEAD
int main() {
    initscr();
    cbreak();
    WINDOW *win, *score;
    ncursesSetup(&win, &score);

    struct Position *drone_pos = malloc(sizeof(struct Position));

    

    // Shared memory segment for drone position
    // initSemaphore();

    // int shmid_pos = getSharedMemorySegment(KEY_POS, sizeof(struct Position));
    // struct Position *drone_pos = attachSharedMemory(shmid_pos);
    
    // //Initialize positions
    // drone_pos->x = COLS/2;
    // drone_pos->y = LINES/2;
    // printf("%d\n", drone_pos->x);
    // printw("%d\n", drone_pos->x);
    
    sem_t *sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_init(sem_id, 1, 0); //initialized to 0 until shared memory is instantiated


    int shmfd  = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
=======

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
>>>>>>> master
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
<<<<<<< HEAD
    // truncate size of shared memory
    ftruncate(shmfd, 1*sizeof(struct Position));
    // map pointer
    drone_pos = (struct Position *)mmap(NULL, sizeof(struct Position), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (drone_pos == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(EXIT_FAILURE);
    }
    
    drone_pos->x = COLS/2;
    drone_pos->y = LINES/2;
    int x,y,key;
    x=COLS/2;
    y=LINES/2;
    int counter=0;
    while (1) {
        // mvwprintw(win,y,x, "X");
        // wrefresh(win);
        // // wrefresh(score);
        
        // refresh();
        // key = wgetch(win);
        
        // // if (key=='o' || key=='l' || key=='.' || key=='r' || key=='f' || key=='v'){ //right or diagonalRight
        // //     x=x+1;
        // // }if(key=='u'|| key=='j' || key=='m' || key=='w' || key=='s' || key=='x'){ //left or diagonalLeft
        // //     x=x-1;
        // // }if(key == 'u'|| key=='i' || key=='o' || key=='w' || key=='e' || key=='r'){ //up
        // //     y=y-1;
        // // }if(key == 'm'|| key==',' || key=='.' || key=='x' || key=='c' || key=='v') // down
        // //     y=y+1;

        // clear();
        if (counter==0){
            mvwprintw(win,drone_pos->y,drone_pos->x, "X");
            counter++;
            wrefresh(win);
            // wrefresh(score);
            
            // refresh();
            drone_pos->key = getch();
            clear();
        }else{
            sem_wait(sem_id);
            // Update window with drone position
            mvwprintw(win,drone_pos->y,drone_pos->x, "O");
            // mvwprintw(win,LINES/2,COLS/2, "X");
            wrefresh(win);
            // wrefresh(score);
            
            // refresh();
            drone_pos->key = getch();
            clear();
            sem_post(sem_id); 
        }
        
    }


        if (munmap(drone_pos, sizeof(struct Position)) == -1) {
            perror("Error unmapping shared memory");
            exit(EXIT_FAILURE);
        }

        // Close shared memory object
        close(shmfd);

        // Close and unlink the semaphore
        sem_close(sem_id);
        sem_unlink("/my_semaphore");

    // Detach shared memory
    // detachSharedMemory(drone_pos);
=======
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
        // wprintw(win,"%d,%d,%f,%f,%f,%f",COLS,LINES,scalex,scaley,position[5],position[5]/scaley);

        // move to the desired position and print "X", 

        // gets the semaphore number. use it to check for deadlock
        int sem;
        sem_getvalue(sem_id,&sem);


        mvwprintw(win, (int)(position[5]/scaley), (int)(position[4]/scalex), "X");
        mvwprintw(score,1,20,"%f,%f", position[4],position[5]);
        // mvwprintw(win, 5, 5, "X");
        wrefresh(win);
        wrefresh(score);
        
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
>>>>>>> master

    endwin();

    return 0;
}