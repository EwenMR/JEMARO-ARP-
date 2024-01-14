#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <unistd.h>
#include <sys/types.h>
#include "../include/log.c"
#include "../include/constants.h"
#include "../include/utility.c"


// Global variables
int all_timer[NUM_PROCESSES-1];
pid_t all_pids[NUM_PROCESSES];

void kill_all(){
    for(int i=0; i<(NUM_PROCESSES-1); i++){
            kill(all_pids[i],SIGINT);
        }
    writeToLogFile(wdlogpath,"kill all");
    exit(1);
}

void send_signals(){
    for(int i=0; i<(NUM_PROCESSES-1); i++){
        if(kill(all_pids[i],SIGUSR1)==0){

        }else{
            kill_all();
            exit(1);
        }
        usleep(50000);
        usleep(50000);
        // kill(all_pids[i],SIGUSR1);
    }
    writeToLogFile(wdlogpath,"sent signals to all processes");
}

int main(int argc, char* argv[]){
    // SIGNALS
    clearLogFile(wdlogpath);


    // INITIALIZATION
    int first=0;
    printf("%d\n",NUM_PROCESSES-1);
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
        // char message[80]= "PID %d = %d",i,all_pids[i];
        // writeToLogFile(wdlogpath,message);
        printf("PID %d = %d\n",i,all_pids[i]);
        // writeToLogFile(wdlogpath,"PID %d = %d",all_pids[i]);

    }



    while(1){
        send_signals();
        printf("Signal sent\n");
        for(int i=0; i<NUM_PROCESSES-1; i++){
            all_timer[i]++;
        }

        // Send signals to all processes
        // for(int i=0; i<(NUM_PROCESSES-1); i++){
        //     kill(all_pids[i],SIGUSR1);
        //     // usleep(100000);
        //     // usleep(50000);
        // }
        
        
        int kill = 0;  // Initialize as false
        for (int i = 0; i < (NUM_PROCESSES-1); i++) {
            if (all_timer[i] > WD_TIMER_THRESH) {
                writeToLogFile(wdlogpath, "Exeeded threshold");
                kill = 1;  // Set to true
                break;  // No need to check further, we found one
            }
        }

        // Use the result in your conditional statement
        if (kill) {
            printf("kill\n");
            kill_all();
            // writeToLogFile(wdlogpath, "Assassin mode ON");
            exit(1);
        }
    }

    return 0;
}

