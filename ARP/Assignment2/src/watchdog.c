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
#define TIMER_THRESH 5

// Global variables
int server_timer, window_timer, keyboard_timer,drone_timer,obstacle_timer,target_pid;
pid_t server_pid, window_pid, keyboard_pid, drone_pid, wd_pid, obstacle_pid, target_pid;

void kill_all(){
    kill(server_pid, SIGINT);
    kill(window_pid, SIGINT);
    kill(keyboard_pid, SIGINT);
    kill(drone_pid, SIGINT);
    kill(wd_pid,SIGINT);
    kill(obstacle_pid, SIGINT);
    kill(target_pid, SIGINT);
    printf("sent signals to all processes\n");
    exit(1);
}

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    printf("Received signal from %d\n", signo);

    if(signo == SIGINT){
        kill_all();
    }
    if(signo == SIGUSR2){
        if(siginfo->si_pid == server_pid){
            printf("Signal sent from SERVER\n");
            server_timer=0;
        }
        if(siginfo->si_pid == window_pid){
            printf("Signal sent from WINDOW\n");
            window_timer=0;
        }
        if(siginfo->si_pid == keyboard_pid){
            printf("Signal sent from KEYBOARD\n");
            keyboard_timer=0;
        }
        if(siginfo->si_pid == drone_pid){
            printf("Signal sent from DRONE\n");
            drone_timer=0;
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
    server_timer = window_timer = drone_timer = keyboard_timer = 0;


     // PIPES
    int server_wd[2];
    
    sscanf(argv[1], "%d %d", &server_wd[0], &server_wd[1]);
    printf("%d %d",  server_wd[0], server_wd[1]);

    close(server_wd[1]);

    pid_t wd_pid, all_pids[NUM_PROCESSES];
    wd_pid=getpid();
    my_read(server_wd[0], all_pids, server_wd[0]);

    //THIS PART WILL NOT BE NECESSARY
    server_pid = all_pids[0];
    window_pid = all_pids[1];
    keyboard_pid = all_pids[2];
    drone_pid = all_pids[3];
    obstacle_pid = all_pids[4];
    target_pid = all_pids[5];

    for(int i=0; i<NUM_PROCESSES; i++){
        printf("%d\n", all_pids[i]);
    }

    while(1){
        sleep(1);
    }
    // printf("window: %d\n",window_pid);
    // printf("server: %d\n",server_pid);
    // printf("drone: %d\n",drone_pid);
    // printf("watchdog: %d\n",wd_pid);





    
    // while(1){
    //     server_timer++;
    //     window_timer++;
    //     drone_timer++;
    //     // keyboard_timer++;


    //     // Send signals to all processes
    //     kill(server_pid,SIGUSR1);
    //     usleep(50000);
    //     kill(window_pid,SIGUSR1);
    //     usleep(50000);
    //     // kill(keyboard_pid,SIGUSR1);
    //     usleep(50000);
    //     usleep(50000);
    //     usleep(50000);
    //     kill(drone_pid,SIGUSR1);
    //     usleep(50000);

    //     if(server_timer > TIMER_THRESH || window_timer > TIMER_THRESH || drone_timer > TIMER_THRESH  || keyboard_timer > TIMER_THRESH){
    //             wd_pid=getpid();
    //             kill_all();
    //             // kill(keyboard_pid,SIGINT);
                
    //             exit(1);
            
    //     }
  
    // }

    
    

    return 0;
}