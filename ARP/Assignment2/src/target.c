#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/utility.c"
#include "../include/constants.h"
#include <time.h>
#include <math.h>
#include "../include/log.c"



// Global variables
double target_pos[NUM_TARGETS*2],obstacle_pos[NUM_OBSTACLES*2],drone_pos[6];
struct shared_data data;

// Signal handler for watchdog
void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        writeToLogFile(targetlogpath,"killed");
        exit(1);
        
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        // kill(wd_pid, SIGUSR2);
        writeToLogFile(targetlogpath,"signal received");
    }
}



// Checks if target is reached
void target_update(double *drone_pos, double *target_pos) {
    int j=0;
    for (int i = 0; i < NUM_TARGETS * 2; i += 2) {
        if(target_pos[i]==0 &&target_pos[i+1]==0){ // Only check the targets that are not 0,0
        }else{
            j=i;
            break;
        }
    }
    
    // Check if the first target (out of the unreached targets) is reached
    if ((fabs(drone_pos[4] - target_pos[j]) < POSITION_THRESHOLD &&
        fabs(drone_pos[5] - target_pos[j + 1]) < POSITION_THRESHOLD) ||
        (fabs(drone_pos[2] - target_pos[j]) < POSITION_THRESHOLD &&
        fabs(drone_pos[3] - target_pos[j + 1]) < POSITION_THRESHOLD)) {

        // Reached targets will be 0,0
        target_pos[j] = 0;
        target_pos[j+1] = 0;

        
    }
}


// make the targets coordinates
void makeTargs(double drone_pos[]){
    for (int i=0; i< NUM_TARGETS*2; i+=2){
        target_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;
        target_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;

        // check if they aren't within threshold of drone
        while(fabs(target_pos[i] - drone_pos[4]) < TARGET_THRESH  && fabs(target_pos[i+1] - drone_pos[5]) < TARGET_THRESH) {
            // Regenerate target_pos coordinates
            target_pos[i] = ((double)rand() / RAND_MAX) * BOARD_SIZE;
            target_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;
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
    clearLogFile(targetlogpath);

    // PIPES
    int target_server[2], server_target[2];
    sscanf(argv[1], args_format,  &target_server[0], &target_server[1], &server_target[0], &server_target[1]);
    close(target_server[0]); //Close unnecessary pipes
    close(server_target[1]);

    // Pids for Watchdog
    pid_t target_pid;
    target_pid=getpid();
    // printf("target PID: %d\n", target_pid);
    my_write(target_server[1], &target_pid, target_server[0],sizeof(target_pid));
    char logMessage[80];
    sprintf(logMessage, "PID = %d\n",target_pid);
    writeToLogFile(targetlogpath, logMessage);


    // Get the initial drone position from drone
    my_read(server_target[0],&data,target_server[1],sizeof(data));
    memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));  

    // Generate targets (only once) and send it to obstacle
    makeTargs(drone_pos); 
    memcpy(data.target_pos, target_pos,sizeof(target_pos));
    write(target_server[1],&data,sizeof(data));
    writeToLogFile(targetlogpath, "TARGET: Initial target_pos sent to server");

    while(1){
        // Receive drone position from drone
        my_read(server_target[0],&data,target_server[1],sizeof(data));
        memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        writeToLogFile(targetlogpath, "TARGET: drone_pos received from server");
        
        target_update(drone_pos, target_pos); // Check if drone reached the target

        // copy updated target position to shared data and send it
        memcpy(data.target_pos, target_pos, sizeof(target_pos));
        my_write(target_server[1], &data, server_target[0], sizeof(data));
        writeToLogFile(targetlogpath, "TARGET: Updated target_pos sent to server");

    }
    // Clean up
    close(target_server[0]);
    close(server_target[1]);
}

