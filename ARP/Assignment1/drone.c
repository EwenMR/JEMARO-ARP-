#include <ncurses.h>
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "include/constants.h"
#include "shared_memory.c"
#define M 1.0
#define K 1.0
#define FORCEX 1.0
#define FORCEY 1.0

double calc_acceleration(double force,double velocity){
    double acceleration;
    acceleration = (force-K*velocity)/M;
    velocity+=acceleration;

    return velocity;
}

int main(int argc, char *argv[]) {
    // xy = force direction of x,y as such -> [0,1]
    int xy[2];
    // initialize velocity
    double velx,vely;
    velx=vely=0;

    // pipe with keyboard
    int keyboard_drone[2];
    sscanf(argv[1],"%d %d", &keyboard_drone[0], &keyboard_drone[1]);
    close(keyboard_drone[1]);

    // make the read non blocking
    int flags = fcntl(keyboard_drone[0], F_GETFL);
    fcntl(keyboard_drone[0], F_SETFL, flags | O_NONBLOCK);


    // Initialize ncurses
    initscr();
    // Don't display cursor
    curs_set(0);

    // Shared memory stuff
    // double position[6] = {COLS/2, LINES/2, COLS/2, LINES/2, COLS/2, LINES/2};
    double position[6];
    int shared_seg_size = (1 * sizeof(position));
    sem_t * sem_id = sem_open(SEM_PATH, 0);

    int shmfd  = shm_open(SHM_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_wait(sem_id);
        memcpy(position,shm_ptr,shared_seg_size);
        sem_post(sem_id);
        // reads message sent from keyboard
        ssize_t bytesRead = read(keyboard_drone[0], xy, sizeof(xy));
        if (bytesRead<0){
            if (errno != EAGAIN) {
                perror("reading error");
                close(keyboard_drone[0]);
                exit(EXIT_FAILURE);
            }
        }else if(bytesRead>0){
            velx=calc_acceleration(FORCEX*xy[0],velx);
            vely=calc_acceleration(FORCEY*xy[1],vely);
            printw("%d %d", xy[0],xy[1]);
            refresh();
        }
        // ssize_t bytesRead = read(keyboard_drone[0], xy, sizeof(xy));

        
        
        // dispaly the 3 positions onto the screen
        int row, col;
        getmaxyx(stdscr, row, col);
        

        // MODIFY IT SO THAT IT MOVES EVERY SECOND
        // update position
        for(int i=0; i<4; i++){
            position[i]=position[i+2];
        }
        position[4]+=velx;
        position[5]-=vely;
        sleep(1);
        
        

        sem_wait(sem_id);
        memcpy(shm_ptr, position, shared_seg_size);
        sem_post(sem_id);

        mvprintw(row / 2, 0, "Before2:%f %f Before1:%f %f  Now: %f %f",position[0],position[1],position[2], position[3],position[4],position[5]);
        refresh();

        // Refresh the screen
        
    }

    close(keyboard_drone[0]);
    shm_unlink(SHM_PATH);
    sem_close(sem_id);
    sem_unlink(SEM_PATH);


    // Print "hello" in the center of the screen
    

    // End ncurses
    endwin();

    return 0;
}
