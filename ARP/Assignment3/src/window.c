
// window.c
#include <signal.h>
#include <curses.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include "../include/utility.c"
#include "../include/log.c"
#include "../include/constants.h"

// Signal handler for watchdog
void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        writeToLogFile(windowlogpath,"killed");
        exit(1);
        
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
        writeToLogFile(windowlogpath,"signal received");
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

struct timeval current_time() {
    /*
    Returns the current time using gettimeofday.
    */
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    return time_now;
}

int calc_score(struct timeval start, struct timeval finish) {
    // Calculate the difference in seconds between two timeval structs
    int time_passed = (int)(finish.tv_sec - start.tv_sec);
    if(time_passed<10){
        return 100;
    }else{
        return 110 - time_passed;
    }
    
}





int main(int argc, char* argv[]) {
    // INITIALIZATION
    initscr();
    noecho();
    cbreak();
    struct timeval start_time, finish_time;
    start_time = current_time();
    clearLogFile(windowlogpath);

    // WINDOW INITIALIZATION
    WINDOW *win, *score;
    ncursesSetup(&win, &score);
    curs_set(0); // don't show cursor
    nodelay(win, TRUE);

    int ymax, xmax;
    getmaxyx(stdscr, ymax, xmax);
    

    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    // PIPES
    int window_server[2], server_window[2];
    sscanf(argv[1], "%d %d|%d %d",  &window_server[0], &window_server[1], &server_window[0], &server_window[1]);
    close(server_window[1]); // Close unnecessary pipes
    close(window_server[0]);

    // Pids for Watchdog
    pid_t window_pid;
    window_pid=getpid();
    my_write(window_server[1], &window_pid, server_window[0],sizeof(window_pid));
    char logMessage[80];
    sprintf(logMessage, "PID = %d\n",window_pid);
    writeToLogFile(windowlogpath, logMessage);

    // Local variables
    struct shared_data data;
    float drone_pos[6]; //position of the drone (t-2,t-1,t)
    float target_pos[NUM_TARGETS*2];
    float obstacle_pos[NUM_OBSTACLES*2];
    int key;
    int first=0;
    
    // Color of the drone, targets, obstacles
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    bool started;
    started = false;
    bool game_set;
    game_set=false;
    
    while (1) {
        werase(win);
        werase(score);
        box(win, 0, 0);
        box(score, 0, 0);
        

        int ymax_new, xmax_new;
        getmaxyx(stdscr, ymax_new, xmax_new);
        data.windowx=(double)xmax*(WINDOW_COL-0.01);
        data.windowy=(double)ymax*(WINDOW_ROW-0.01);
        sprintf(logMessage,"%d %d",data.windowx,data.windowy);
        if (started==false){
            writeToLogFile(windowlogpath,logMessage);
            my_write(window_server[1],&data,window_server[0],sizeof(data));
        }
        


        // Change the scale if the window size changes
        if(ymax != ymax_new || xmax != xmax_new){
            wresize(win, ymax_new*WINDOW_ROW, xmax_new*WINDOW_COL);
            wresize(score, ymax_new *SCORE_WINDOW_ROW, xmax_new *WINDOW_COL);
            mvwin(score, (ymax_new / 200) + ymax_new * WINDOW_ROW, xmax_new / 200);

            wclear(win);
            wclear(score);
            // Redraw boxes
            box(win, 0, 0);
            box(score, 0, 0);

            refresh();
            wrefresh(win);
            wrefresh(score);

            ymax = ymax_new;
            xmax = xmax_new;
        }


        

        // READ SHARED DATA and store it into local variables.
        my_read(server_window[0],&data,server_window[1],sizeof(data));
        memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        memcpy(target_pos, data.target_pos, sizeof(data.target_pos));
        memcpy(obstacle_pos, data.obst_pos, sizeof(data.obst_pos));
        writeToLogFile(windowlogpath, "WINDOW: Drone_pos, Target_pos, Obst_pos received from server");
        sprintf(logMessage, "T:%f %f O:%f %f", data.target_pos[0],data.target_pos[1],data.obst_pos[0],data.obst_pos[1]);
        writeToLogFile(windowlogpath,logMessage);
        
        if(target_pos[0]!=(float)(0) && target_pos[1]!=(float)(0)){
            started=true;
            writeToLogFile(windowlogpath,"STARTED");
        }

        double scalex,scaley; // get the scale, to scale up the window to the real world scale
        scalex=(double)BOARD_SIZE /((double)xmax*(WINDOW_COL-0.01));
        scaley=(double)BOARD_SIZE/((double)ymax*(WINDOW_ROW-0.01));

        // Print drone and score onto the window
        wattron(win,COLOR_PAIR(1));
        mvwprintw(win, (int)(drone_pos[5]/scaley), (int)(drone_pos[4]/scalex), "+");
        wattroff(win,COLOR_PAIR(1));

        // Print obstacles on window
        wattron(win,COLOR_PAIR(3));
        for(int i=0; i<(NUM_OBSTACLES*2);i+=2){
            mvwprintw(win, (int)(obstacle_pos[i+1]), (int)(obstacle_pos[i]), "O");
        }
        wattroff(win,COLOR_PAIR(3));

        // Print targets on window
        wattron(win,COLOR_PAIR(2));
        for(int i=2; i<(NUM_TARGETS*2)+2;i+=2){
            if(target_pos[i-2]==0 && target_pos[i-1]==0){
            }else{
                mvwprintw(win, (int)(target_pos[i-1]), (int)(target_pos[i-2]), "%d", i/2);
            }
        }
        wattroff(win,COLOR_PAIR(2));

        // Display drone on window
        mvwprintw(score,1,1,"Position of the drone is: %f,%f", drone_pos[4],drone_pos[5]);
        wrefresh(win);
        wrefresh(score);
        
        // If all targets are reached, Game finished
        if(started == true){
            game_set = true;
        }
        for (int i=0; i<NUM_TARGETS*2; i++){
            if(target_pos[i]!=0){
                game_set=false;
            }
        }
        if(game_set==true){
            finish_time = current_time();
            int your_score = calc_score(start_time, finish_time);

            werase(win);
            box(win, 0, 0);
            mvwprintw(win, LINES/2, COLS/2, "SCORE IS %d", your_score);
            wrefresh(win);
            sleep(2);
            exit(EXIT_SUCCESS);
        }
        
        // Send user input to keyboard manager
        key=wgetch(win); // wait for user input
        if (key != ERR) { // If a key was pressed properly
            data.key=key;
            my_write(window_server[1],&data,window_server[0],sizeof(data));
            writeToLogFile(windowlogpath, "WINDOW: User input sent to server");
            // if((char)key==' '){ //if space was pressed, close window
            //     close(window_server[1]);
            //     close(server_window[0]);
            //     exit(EXIT_SUCCESS);
            // }
        }
    }

    // Clean up
    close(window_server[1]);
    close(window_server[0]); 

    endwin();

    return 0;
}