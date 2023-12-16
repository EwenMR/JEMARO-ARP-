#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>


#include "../include/constants.h"
#include "../include/utility.c"

#define THRESHOLD 5.0 

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


void makeObs(double drone_pos[]) {
    for (int i = 0; i < NUM_OBSTACLES*2; i+=2) {
        // generating obstacle coordinates
        obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
        obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100

        // check if the x obstacle coordinate isn't too close to drone
        while (obstacle_pos[i] >= drone_pos[4] - THRESHOLD && obstacle_pos[i] <= drone_pos[4] + THRESHOLD && obstacle_pos[i+1] >= drone_pos[5] - THRESHOLD && obstacle_pos[i+1] <= drone_pos[5] + THRESHOLD) {
            // Regenerate obstacle-coordinate
            obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
            obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
        }
        for(int j=0; i<NUM_TARGETS*2; i+=2){
            while (obstacle_pos[i] >= target_pos[i] - THRESHOLD && obstacle_pos[i] <= target_pos[i] + THRESHOLD && obstacle_pos[i+1] >= target_pos[i+1] - THRESHOLD && obstacle_pos[i+1] <= target_pos[i+1] + THRESHOLD) {
            // Regenerate obstacle-coordinate
            obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
            obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
        }
        }
        
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
    char args_format[80]="%d %d|%d %d";
    sscanf(argv[1], args_format,  &obstacle_server[0], &obstacle_server[1], &server_obstacle[0], &server_obstacle[1]);
    // close(obstacle_server[0]); //Close unnecessary pipes
    // close(server_obstacle[1]);

    pid_t obstacle_pid;
    obstacle_pid=getpid();
    write(obstacle_server[1], &obstacle_pid, sizeof(obstacle_pid));


    struct shared_data data;


    printf("OBSTACLE\n");

    while(1){
        my_read(server_obstacle[0],&data,obstacle_server[1],sizeof(data));
        memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        memcpy(target_pos, data.target_pos, sizeof(data.target_pos));
        // memcpy(obstacle_pos, data.obst_pos, sizeof(data.obst_pos));

        makeObs(drone_pos);
        memcpy(data.obst_pos,obstacle_pos,sizeof(obstacle_pos));
        my_write(obstacle_server[1],&data,server_obstacle[0],sizeof(data));
        printf("obstacle: %f %f %f %f\n",obstacle_pos[0],obstacle_pos[1],obstacle_pos[2],obstacle_pos[3]);

        
        sleep(1);
    }

}






