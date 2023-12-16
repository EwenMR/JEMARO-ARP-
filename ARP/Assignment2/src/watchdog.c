#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <unistd.h>
#include <sys/types.h>

#include "../include/constants.h"
#include "../include/utility.c"
#define TIMER_THRESH 100

// Global variables
int all_timer[NUM_PROCESSES-1];
pid_t all_pids[NUM_PROCESSES];

void kill_all(){
    for(int i=0; i<(NUM_PROCESSES-1); i++){
            kill(all_pids[i],SIGINT);
        }
    printf("sent signals to all processes\n");
    exit(1);
}

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    printf("Received signal\n");

    if(signo == SIGINT){
        kill_all();
    }
    if(signo == SIGUSR2){

        if(siginfo->si_pid == all_pids[0]){
            printf("Signal sent from WINDOW\n");
            all_timer[1]=0;
        }
        if(siginfo->si_pid == all_pids[1]){
            // printf("Signal sent from KEYBOARD\n");
            all_timer[2]=0;
        }
        if(siginfo->si_pid == all_pids[2]){
            // printf("Signal sent from DRONE\n");
            all_timer[3]=0;
        }if(siginfo->si_pid == all_pids[3]){
            // printf("Signal sent from OBSTACLE\n");
            all_timer[4]=0;
        }
        if(siginfo->si_pid == all_pids[4]){
            // printf("Signal sent from TARGET\n");
            all_timer[5]=0;
        }if(siginfo->si_pid == all_pids[5]){
            // printf("Signal sent from pid %d\n", all_pids[0]);
            all_timer[0]=0;
        }
    }
}

int main(int argc, char* argv[]){
    // SIGNALS
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT,&sig_act, NULL);
    sigaction(SIGUSR2, &sig_act, NULL);


    // INITIALIZATION
    int first=0;
    printf("%d\n",NUM_PROCESSES-1);
    // server_timer = window_timer = drone_timer = keyboard_timer = 0;
    for (int i=0; i<NUM_PROCESSES-1;i++){
        all_timer[i]=0;
    }

     // PIPES
    int server_wd[2];
    sscanf(argv[1], "%d %d", &server_wd[0], &server_wd[1]);

    // close(server_wd[1]);

    
    my_read(server_wd[0], all_pids, server_wd[1], sizeof(all_pids));
    all_pids[NUM_PROCESSES-1]=getpid();

    for(int i=0; i<NUM_PROCESSES; i++){
        // printf("%d\n", all_pids[i]);
    }



    while(1){
        for(int i=0; i<NUM_PROCESSES-1-2; i++){
            all_timer[i]++;
        }

        // Send signals to all processes
        for(int i=0; i<(NUM_PROCESSES-1); i++){
            kill(all_pids[i],SIGUSR1);
            // usleep(50000);
            // usleep(50000);
        }
        
        //MAKE THIS CODE CLEANER
        int kill = 0;  // Initialize as false
        for (int i = 0; i < (NUM_PROCESSES-1); ++i) {
            if (all_timer[i] > TIMER_THRESH) {
                kill = 1;  // Set to true
                break;  // No need to check further, we found one
            }
        }

        // Use the result in your conditional statement
        if (kill) {
            printf("kill\n");
            // kill_all();
        }
    }

    return 0;
}

