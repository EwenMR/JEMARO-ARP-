#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
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
    while(1){

    }

}




// for INSPO

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define THRESHOLD 10          // threshold distance for the closeness an obstacle canbe to the drone
#define NUM_TAR 7               // number of targets
#define MAX_TAR_ARR_SIZE  20    // max array size for targets


// struct for the targets
typedef struct {
    double x;  
    double y;  
} Target;


// print the coordinates for value checking
void printTargs( Target tar[]){
    for ( int i=0; i < NUM_TAR; i++){
        printf("Target %d: x = %.3f, y = %.3f\n", i+1, tar[i].x, tar[i].y);
    }
}

// make the target coordinates
void makeTargs (Target tar[], double droneX, double droneY){
    for (int i=0; i< NUM_TAR; i++){
        tar[i].x = ((double)rand() / RAND_MAX) * 500.0;
        tar[i].y = ((double)rand() / RAND_MAX) * 500.0;

    // check if they aren't within threshold of drone
    while (tar[i].x >= droneX - THRESHOLD && tar[i].x <= droneX + THRESHOLD) {
            // Regenerate x-coordinate
            tar[i].x = ((double)rand() / RAND_MAX) * 500.0;
        }

    while (tar[i].y >= droneY - THRESHOLD && tar[i].y <= droneY + THRESHOLD) {
        // Regenerate y-coordinate
        tar[i].y = ((double)rand() / RAND_MAX) * 500.0;
        }

    }
}


int main(){

    // seeds the random number generator
    srand(time(NULL));

    // examples starting pos
    double droneX = 50;
    double droneY = 50;

    // declare array, make the targets and print them
    Target tar[NUM_TAR];
    makeTargs(tar, droneX, droneY);
    printTargs(tar);
}
