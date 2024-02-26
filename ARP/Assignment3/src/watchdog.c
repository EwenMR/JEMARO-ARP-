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
// Send signals to all children processes to terminate
void kill_all(){
    for(int i=0; i<(NUM_PROCESSES-1); i++){
            kill(all_pids[i],SIGINT);
        }
    writeToLogFile(wdlogpath,"kill all");
    // exit(0);
}


void signal_handler(int signo, siginfo_t *siginfo, void *context){
    printf("Received signal\n");

    if(signo == SIGINT){
        kill_all();
        exit(1);
    }
    if(signo == SIGUSR2){

        if(siginfo->si_pid == all_pids[0]){
            sprintf(logMessage, "Signal sent from Window");
            writeToLogFile(wdlogpath, logMessage);

            all_timer[1]=0;
        }
        if(siginfo->si_pid == all_pids[1]){
            sprintf(logMessage, "Signal sent from Keyboard");
            writeToLogFile(wdlogpath, logMessage);

            all_timer[2]=0;
        }
        if(siginfo->si_pid == all_pids[2]){
            sprintf(logMessage, "Signal sent from Drone");
            writeToLogFile(wdlogpath, logMessage);

            all_timer[3]=0;
        }if(siginfo->si_pid == all_pids[3]){
            sprintf(logMessage, "Signal sent from Obstacle");
            writeToLogFile(wdlogpath, logMessage);

            all_timer[4]=0;
        }
        if(siginfo->si_pid == all_pids[4]){
            sprintf(logMessage, "Signal sent from Target");
            writeToLogFile(wdlogpath, logMessage);

            all_timer[5]=0;
        }if(siginfo->si_pid == all_pids[5]){
            sprintf(logMessage, "Signal sent from Server");
            writeToLogFile(wdlogpath, logMessage);

            all_timer[0]=0;
        }
    }
}






int main(int argc, char* argv[]){
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT,&sig_act, NULL);
    sigaction(SIGUSR2, &sig_act, NULL);

    clearLogFile(wdlogpath);

     // PIPES
    int server_wd[2];
    sscanf(argv[1], "%d %d", &server_wd[0], &server_wd[1]);
    
    // Read and store all pids of children processes
    my_read(server_wd[0], all_pids, server_wd[1], sizeof(all_pids));
    all_pids[NUM_PROCESSES-1]=getpid();

    // Log all pids to the logfile
    char logMessage[80];
    sprintf(logMessage, "WD PID %d\n",all_pids[NUM_PROCESSES-1]);
    writeToLogFile(wdlogpath, logMessage);
    for(int i=0; i<NUM_PROCESSES; i++){
        char logMessage[80];
        sprintf(logMessage, "PID %d = %d\n",i,all_pids[i]);
        writeToLogFile(wdlogpath, logMessage);
    }

    // INITIALIZATION
    printf("%d\n",NUM_PROCESSES-1);
    for (int i=0; i<NUM_PROCESSES-1;i++){
        all_timer[i]=0;
    }

    while(1){
        // Send signals to all processes
        for(int i=0; i<(NUM_PROCESSES-1); i++){
            if(kill(all_pids[i],0)==0){ //If signal is properly sent
                all_timer[i]=0;
                sprintf(logMessage, "Signal sent to %d\n",all_pids[i]);
                writeToLogFile(wdlogpath, logMessage);

            }else{ // If signal is not received by any of the children processes
                all_timer[i]++;
                sprintf(logMessage, "Signal not sent to %d\n",all_pids[i]);
                writeToLogFile(wdlogpath, logMessage);
            }
            usleep(100000);
        }
        
        
        int kill = 0;  // Initialize as false
        for (int i = 0; i < (NUM_PROCESSES-1); i++) {
            if (all_timer[i] > WD_TIMER_THRESH) { // If any of the processes ignored the signal for more than threshold
                writeToLogFile(wdlogpath, "Exeeded threshold");
                kill = 1;  // Set kill to true
                break;  // No need to check further, we found one
            }
        }

        // Kill all processes
        if (kill==1) {
            printf("kill\n");
            kill_all();
            writeToLogFile(wdlogpath, "Killed all processes");
            exit(1);
        }
    }
    return 0;
}




