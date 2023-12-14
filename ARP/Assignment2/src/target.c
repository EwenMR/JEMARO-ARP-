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
#define THRESHOLD 10          // threshold distance for the closeness an obstacle canbe to the drone
#define NUM_TAR 7               // number of targets
#define MAX_TAR_ARR_SIZE  20    // max array size for targets



double drone_pos[6], target_pos[NUM_TARGETS*2],obstacle_pos[NUM_OBSTACLES*2];

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



// make the targets coordinates
void makeTargs (double targets[], double drone_pos[]){
    for (int i=0; i< NUM_TARGETS*2; i+=2){
        targets[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;
        targets[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;

        // check if they aren't within threshold of drone
        while (targets[i] >= drone_pos[4] - THRESHOLD && targets[i] <= drone_pos[4] + THRESHOLD && targets[i+1] >= drone_pos[5] - THRESHOLD && targets[i+1] <= drone_pos[5] + THRESHOLD) {
            // Regenerate targets coordinates
            targets[i] = ((double)rand() / RAND_MAX) * BOARD_SIZE;
            targets[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;
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
    int target_server[2], server_target[2];
    char args_format[80]="%d %d|%d %d";
    sscanf(argv[1], args_format,  &target_server[0], &target_server[1], &server_target[0], &server_target[1]);
    // close(target_server[0]); //Close unnecessary pipes
    close(server_target[1]);

    pid_t target_pid;
    target_pid=getpid();
    my_write(target_server[1], &target_pid, target_server[0],sizeof(target_pid));
    printf("%d\n",target_pid);

    struct shared_data data;
    

    while(1){
        // Get shared data and store it into local variables
        my_read(server_target[0],&data,target_server[1],sizeof(data));
        memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        memcpy(target_pos, data.target_pos, sizeof(data.target_pos));
        memcpy(obstacle_pos, data.obst_pos, sizeof(data.obst_pos));

        makeTargs(target_pos, drone_pos);

        // copy target position to shared data and send it
        memcpy(data.target_pos, target_pos, sizeof(target_pos));
        my_write(target_server[1], &data, server_target[0], sizeof(data));

    }

}



// struct for the targets
// typedef struct {
//     double x;  
//     double y;  
// } Target;


// // print the coordinates for value checking
// void printTargs( Target tar[]){
//     for ( int i=0; i < NUM_TAR; i++){
//         printf("Target %d: x = %.3f, y = %.3f\n", i+1, tar[i].x, tar[i].y);
//     }
// }

// // make the target coordinates
// void makeTargs (Target tar[], double droneX, double droneY){
//     for (int i=0; i< NUM_TAR; i++){
//         tar[i].x = ((double)rand() / RAND_MAX) * BOARD_SIZE;
//         tar[i].y = ((double)rand() / RAND_MAX) * BOARD_SIZE;

//         // check if they aren't within threshold of drone
//         while (tar[i].x >= droneX - THRESHOLD && tar[i].x <= droneX + THRESHOLD && tar[i].y >= droneY - THRESHOLD && tar[i].y <= droneY + THRESHOLD) {
//             // Regenerate target coordinates
//             tar[i].x = ((double)rand() / RAND_MAX) * BOARD_SIZE;
//             tar[i].y = ((double)rand() / RAND_MAX) * BOARD_SIZE;
//         }
//     }
// }




// int main(){

//     // seeds the random number generator
//     srand(time(NULL));

//     // examples starting pos
//     double droneX = 50;
//     double droneY = 50;

//     // declare array, make the targets and print them
//     Target tar[NUM_TAR];
//     makeTargs(tar, droneX, droneY);
//     printTargs(tar);
// }



