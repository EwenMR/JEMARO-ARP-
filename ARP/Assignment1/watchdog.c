#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <unistd.h>
#include <sys/types.h>

#include "include/constants.h"
#define TIMER_THRESH 5

// Global variables
int server_timer, window_timer, keyboard_timer,drone_timer;
pid_t server_pid, window_pid, keyboard_pid, drone_pid, wd_pid, pid_kb;

void kill_all(){
    kill(server_pid, SIGINT);
    kill(window_pid, SIGINT);
    kill(drone_pid, SIGINT);
    kill(pid_kb,SIGINT);
    kill(keyboard_pid, SIGINT);
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

    // INITIALIZATION
    int wd_server[2],wd_window[2],wd_drone[2],wd_keyboard[2],key_wd[2];
    int first=0;
    server_timer = window_timer = drone_timer = keyboard_timer = 0;


    // GET PIDS OF ALL PROCESSES
    sscanf(argv[1],"%d %d|%d %d|%d %d|%d %d|%d", &wd_server[0], &wd_server[1], &wd_window[0],&wd_window[1], &wd_keyboard[0], &wd_keyboard[1], &wd_drone[0],&wd_drone[1], &pid_kb);
    close(wd_server[1]);
    close(wd_drone[1]);
    close(wd_keyboard[0]);
    close(wd_window[1]);

    wd_pid=getpid();
    read(wd_drone[0], &drone_pid, sizeof(drone_pid));
    // write(key_wd[1], &wd_pid, sizeof(wd_pid));
    read(wd_window[0], &window_pid, sizeof(window_pid));
    read(wd_server[0], &server_pid, sizeof(server_pid));

    printf("window: %d\n",window_pid);
    printf("server: %d\n",server_pid);
    printf("drone: %d\n",drone_pid);
    printf("watchdog: %d\n",wd_pid);

    close(wd_server[0]);
    close(wd_drone[0]);
    close(wd_keyboard[1]);
    close(wd_window[0]);


    // SIGNALS
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT,&sig_act, NULL);
    sigaction(SIGUSR2, &sig_act, NULL);

    
    while(1){
        server_timer++;
        window_timer++;
        drone_timer++;
        // keyboard_timer++;


        // Send signals to all processes
        kill(server_pid,SIGUSR1);
        usleep(50000);
        kill(window_pid,SIGUSR1);
        usleep(50000);
        // kill(keyboard_pid,SIGUSR1);
        usleep(50000);
        usleep(50000);
        usleep(50000);
        kill(drone_pid,SIGUSR1);
        usleep(50000);

        if(server_timer > TIMER_THRESH || window_timer > TIMER_THRESH || drone_timer > TIMER_THRESH  || keyboard_timer > TIMER_THRESH){
                wd_pid=getpid();
                kill_all();
                // kill(keyboard_pid,SIGINT);
                
                exit(1);
            
        }
  
    }

    
    

    return 0;
}