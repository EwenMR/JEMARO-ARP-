
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include "../include/constants.h"
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "../include/utility.c"

// Signal handler for watchdog
void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
    }
}


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
        (LINES/200) +LINES*WINDOW_ROW,
        COLS/200
    };

    *score = create_newwin(LINES*SCORE_WINDOW_ROW, COLS*WINDOW_COL, initPos[2], initPos[3]);
    *display = create_newwin(LINES*WINDOW_ROW, COLS*WINDOW_COL, initPos[0], initPos[1]);
    
}


int main(int argc, char* argv[]) {
    // INITIALIZATION
    printf("THIS IS WINDOW\n");
    initscr();
    int key;
    printf("initialized screen\n");

    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    printf("signal handled\n");

    // PIPES
    int window_server[2], server_window[2];
    char args_format[80]="%d %d|%d %d";
    printf("this is the error\n");
    sscanf(argv[1], "%d %d|%d %d",  &window_server[0], &window_server[1], &server_window[0], &server_window[1]);
    printf("fixed error\n");
    close(window_server[0]); //Close unnecessary pipes
    close(server_window[1]);

    printf("set up pipes\n");

    pid_t window_pid;
    window_pid=getpid();
    printf("NOTHING WRITTEN\n");
    my_write(window_server[1], &window_pid, server_window[0]);
    printf("WRITTEN TO SERVER\n");



    // SHARED MEMORY
    double position[6]={BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2}; //position of the drone (t-2,t-1,t)


    while (1) {
        // refresh window
        WINDOW *win, *score;
        ncursesSetup(&win, &score);
        curs_set(0); // don't show cursor
        nodelay(win, TRUE);

        double scalex,scaley; // get the scale, to scale up the window to the real world scale
        scalex=(double)BOARD_SIZE /((double)COLS*(WINDOW_COL-0.01));
        scaley=(double)BOARD_SIZE/((double)LINES*(WINDOW_ROW-0.01));


        // 1 Send the initial drone position to drone.c via shared memory
        my_write(window_server[1],position,server_window[0]);

        
        // print drone and score onto the window
        mvwprintw(win, (int)(position[5]/scaley), (int)(position[4]/scalex), "X");
        mvwprintw(score,1,1,"Position of the drone is: %f,%f", position[4],position[5]);
        wrefresh(win);
        wrefresh(score);
        
       
        
        // 2 Send user input to keyboard manager
        key=wgetch(win); // wait for user input
        if (key != ERR) { // If a key was pressed properly
            my_write(window_server[1], &key, server_window[0]); //write to keyboard via pipe
            if((char)key==' '){ //if space was pressed, close window
                close(window_server[1]);
                close(server_window[0]);
                exit(EXIT_SUCCESS);
            }
        }
        usleep(200000);


        // 4 Read from shared memory
        my_read(server_window[0], position, window_server[1]);

        clear();
    }

    // Clean up
    close(window_server[1]);
    close(server_window[0]);


    endwin();

    return 0;
}