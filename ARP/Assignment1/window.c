#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

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
    
    // mvwprintw(*display,1,2,"O");
    // wrefresh(*display);
    wrefresh(*display);
    wrefresh(*score);
}

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
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
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

    // Clean up
    endwin();

    return 0;
}
