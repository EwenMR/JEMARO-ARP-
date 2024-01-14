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
    // exit(0);
}

void send_signals(){
    for(int i=0; i<(NUM_PROCESSES-1); i++){
        if(kill(all_pids[i],SIGUSR1)==0){

        }else{
            kill_all();
            // exit(2);
        }
        usleep(50000);
        // usleep(50000);
        // kill(all_pids[i],SIGUSR1);
    }
    writeToLogFile(wdlogpath,"sent signals to all processes");
}

int main(int argc, char* argv[]){
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

    char logMessage[80];
    sprintf(logMessage, "WD PID %d\n",all_pids[NUM_PROCESSES-1]);
    writeToLogFile(wdlogpath, logMessage);



    for(int i=0; i<NUM_PROCESSES; i++){
        // printf();
        char logMessage[80];
        sprintf(logMessage, "PID %d = %d\n",i,all_pids[i]);
        writeToLogFile(wdlogpath, logMessage);

    }



    while(1){

        // Send signals to all processes
        for(int i=0; i<(NUM_PROCESSES-1); i++){

            // int signum=12;
            kill(all_pids[i],SIGUSR1);
            char logMessage[80];
            sprintf(logMessage, "Sent signal to PID %d\n", all_pids[i]);
            writeToLogFile(wdlogpath,logMessage);


            // if(kill(all_pids[i],signum)==0){
            //     all_timer[i]=0;
            // }
            // else{
            //     printf("ERROR with signal\n");
            //     // kill_all();
            //     // exit(2);
            // }
            usleep(500);
        }
        
        

        for(int i=0; i<NUM_PROCESSES-1; i++){
            all_timer[i]++;
        }
    }

    return 0;
}










        // int kill = 0;  // Initialize as false
        // for (int i = 0; i < (NUM_PROCESSES-1); i++) {
        //     if (all_timer[i] > WD_TIMER_THRESH) {
        //         writeToLogFile(wdlogpath, "Exeeded threshold");
        //         kill = 1;  // Set to true
        //         break;  // No need to check further, we found one
        //     }
        // }

        // Use the result in your conditional statement
        // if (kill) {
        //     printf("kill\n");
        //     kill_all();
        //     // writeToLogFile(wdlogpath, "Assassin mode ON");
        //     // exit(3);
        // }