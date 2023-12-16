
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
#include "../include/log.c"

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
    initscr();
    noecho();
    cbreak();
    
    WINDOW *win, *score;
    ncursesSetup(&win, &score);
    curs_set(0); // don't show cursor
    nodelay(win, TRUE);
    

    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    // PIPES
    int window_server[2], server_window[2];
    char args_format[80]="%d %d|%d %d";
    sscanf(argv[1], "%d %d|%d %d",  &window_server[0], &window_server[1], &server_window[0], &server_window[1]);


    pid_t window_pid;
    window_pid=getpid();
    my_write(window_server[1], &window_pid, server_window[0],sizeof(window_pid));
    writeToLogFile(logpath, "WINDOW: Pid sent to server");



    // SHARED MEMORY
    struct shared_data data;
    double drone_pos[6]; //position of the drone (t-2,t-1,t)
    double target_pos[NUM_TARGETS*2];
    double obstacle_pos[NUM_OBSTACLES*2];
    int key;
    int first=0;
    


    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    
    while (1) {
        werase(win);
        box(win, 0, 0);
        // READ SHARED DATA and store it into local variables.

        my_read(server_window[0],&data,server_window[1],sizeof(data));
        writeToLogFile(logpath, "WINDOW: Drone_pos, Target_pos, Obst_pos received from server");
        memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        memcpy(target_pos, data.target_pos, sizeof(data.target_pos));
        memcpy(obstacle_pos, data.obst_pos, sizeof(data.obst_pos));


        
   

        double scalex,scaley; // get the scale, to scale up the window to the real world scale
        scalex=(double)BOARD_SIZE /((double)COLS*(WINDOW_COL-0.01));
        scaley=(double)BOARD_SIZE/((double)LINES*(WINDOW_ROW-0.01));

        // Only for the first time, send the drone position from the window. 
        // From the second time, drone.c will be in charge of drone positions

        
        
        // print drone and score onto the window
        wattron(win,COLOR_PAIR(1));
        mvwprintw(win, (int)(drone_pos[5]/scaley), (int)(drone_pos[4]/scalex), "+");
        wattroff(win,COLOR_PAIR(1));

        wattron(win,COLOR_PAIR(3));
        for(int i=0; i<(NUM_OBSTACLES*2);i+=2){
            mvwprintw(win, (int)(obstacle_pos[i+1]/scaley), (int)(obstacle_pos[i]/scalex), "O");
        }
        wattroff(win,COLOR_PAIR(3));

        wattron(win,COLOR_PAIR(2));
        for(int i=2; i<(NUM_TARGETS*2)+2;i+=2){
            if(target_pos[i-2]==0 && target_pos[i-1]==0){
            }else{
                mvwprintw(win, (int)(target_pos[i-1]/scaley), (int)(target_pos[i-2]/scalex), "%d", i/2);
            }
            
            
        }
        
        if(target_pos[NUM_TARGETS*2-2]==0 && target_pos[NUM_TARGETS*2-1]==0){ //GAME IS FINISHED
            werase(win);
            box(win, 0, 0);
            mvwprintw(win, LINES/2, COLS/2, "WELL DONE");
            wrefresh(win);
            sleep(5);
            exit(EXIT_SUCCESS);
        }
        wattroff(win,COLOR_PAIR(2));

        mvwprintw(score,1,1,"Position of the drone is: %f,%f", drone_pos[4],drone_pos[5]);
        wrefresh(win);
        wrefresh(score);
        
       
        
        // 2 Send user input to keyboard manager
        key=wgetch(win); // wait for user input
        if (key != ERR) { // If a key was pressed properly
            data.key=key;
            my_write(window_server[1],&data,window_server[0],sizeof(data));
            writeToLogFile(logpath, "WINDOW: User input sent to server");
            if((char)key==' '){ //if space was pressed, close window
                close(window_server[1]);
                close(server_window[0]);
                exit(EXIT_SUCCESS);
            }
        }

        clear();
    }

    // Clean up
    close(window_server[1]);
    close(window_server[0]); 
    close(server_window[1]);
    close(server_window[0]);


    endwin();

    return 0;
}