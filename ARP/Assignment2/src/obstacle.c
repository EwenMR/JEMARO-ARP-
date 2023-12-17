#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include "../include/log.c"

#include "../include/constants.h"
#include "../include/utility.c"


// GLOBAL VARIABLES
double drone_pos[6], obstacle_pos[NUM_OBSTACLES*2], target_pos[NUM_TARGETS*2];


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

// Function to generate Obstacles
void makeObs(double drone_pos[]) {
    for (int i = 0; i < NUM_OBSTACLES*2; i+=2) {
        // generating obstacle coordinates
        obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
        obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100

        // check if the x obstacle coordinate isn't too close to drone
        while (obstacle_pos[i]   >= drone_pos[4] - OBS_THRESH && obstacle_pos[i]   <= drone_pos[4] + OBS_THRESH && 
               obstacle_pos[i+1] >= drone_pos[5] - OBS_THRESH && obstacle_pos[i+1] <= drone_pos[5] + OBS_THRESH) {
            // Regenerate obstacle-coordinate
            obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
            obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
        }
        for(int j=0; j<NUM_TARGETS*2; j+=2){
            while (obstacle_pos[i]   >= target_pos[j] - OBS_THRESH   && obstacle_pos[i]   <= target_pos[j] + OBS_THRESH && 
                   obstacle_pos[i+1] >= target_pos[j+1] - OBS_THRESH && obstacle_pos[i+1] <= target_pos[j+1] + OBS_THRESH) {
                // Regenerate obstacle-coordinate
                obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
                obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
            }
        }
    }
}

// Get the current time (only in seconds)
int get_time(){
    int seconds,milliseconds;
    
    time_t t_now;
    time(&t_now);
    struct tm *local = localtime(&t_now);
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    local = localtime(&time_now.tv_sec);
    seconds = local -> tm_sec;

    return seconds;
}

// Check if OBSTACLE_REFRESH_RATE has passed
bool update(int remainder){
    int seconds;
    seconds=get_time();
    
    if(seconds%OBSTACLE_REFRESH_RATE == remainder){ // 
        return true;
    }else{
        return false;
    }
}

int main(int argc, char* argv[]){
    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    // PIPES
    int obstacle_server[2], server_obstacle[2];
    sscanf(argv[1], args_format,  &obstacle_server[0], &obstacle_server[1], &server_obstacle[0], &server_obstacle[1]);
    close(obstacle_server[0]); //Close unnecessary pipes
    close(server_obstacle[1]);

    // Pids for Watchdog
    pid_t obstacle_pid;
    obstacle_pid=getpid();
    write(obstacle_server[1], &obstacle_pid, sizeof(obstacle_pid));
    writeToLogFile(logpath, "OBSTACLE: Pid sent to server");

    // Local variables
    struct shared_data data;
    int seconds,new_seconds,remainder;
    bool first=true;
    bool once =true;
    
    while(1){
        // Receive drone_pos and target_pos from target
        my_read(server_obstacle[0],&data,obstacle_server[1],sizeof(data));
        memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        memcpy(target_pos, data.target_pos, sizeof(data.target_pos));
        writeToLogFile(logpath, "OBSTACLE: Drone_pos and Target_pos received from server");


        if(first){
            makeObs(drone_pos); 
            first=false;
            seconds = get_time();
            remainder = seconds % OBSTACLE_REFRESH_RATE; 
            /*
            ex) first obstacle generated at 7(seconds),
                remainder = 7 % 5 = 2;
                -> Regenerate obstacles when remainder is 2
                -> 7, 12, 17, 22 ....  (5 seconds interval)
            */
            
        }else{
            new_seconds=get_time();
            if(update(remainder)==true && seconds != new_seconds){ // After the && is to prevent obstacles from being generated multiple times within a second
                makeObs(drone_pos);
                seconds=new_seconds;
            }
            
        }
        // Whether newly generated or not, send obstacle_pos to drone
        memcpy(data.obst_pos,obstacle_pos,sizeof(obstacle_pos));
        my_write(obstacle_server[1],&data,server_obstacle[0],sizeof(data));
        writeToLogFile(logpath, "OBSTACLE: Obstacle_pos sent to server");
        
        
        usleep(50000);
    }

}


    







