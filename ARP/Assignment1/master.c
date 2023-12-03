//master.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include "include/constants.h"


void summon(char **programArgs){
    execvp(programArgs[0], programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    // arguments to summon child konsole
    char *argsServer[] = {"konsole", "-e","./build/server", "placeholder",NULL};
    char *argsWindow[] = {"konsole",  "-e", "./build/window", "placeholder",NULL};
    char *argsDrone[] = {"konsole",  "-e", "./build/drone", "placeholder",NULL};
    char *argsKeyboard[] = {"konsole",  "-e","./build/keyboard", "placeholder",NULL};
    char *argsWatchdog[] = {"konsole",  "-e", "./build/watchdog", "placeholder",NULL};


    // fds for pipes
    int window_keyboard[2];
    int keyboard_drone[2];
    if (pipe(window_keyboard) == -1 || pipe(keyboard_drone) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //fds and pipes to send pids to watchdog
    int wd_server[2];
    int wd_window[2];
    int wd_keyboard[2];
    int wd_drone[2];
    if (pipe(wd_drone) == -1 || pipe(wd_keyboard) == -1 || pipe(wd_server) == -1 || pipe(wd_window) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    

    
    //pids of all konsoles
    pid_t all_pid[NUM_PROCESSES];

    for (int i=0; i<NUM_PROCESSES; i++){
        // fork children
        pid_t pid = fork(); 
        all_pid[i]=pid;
        char args[MAX_MSG_LEN];
        if(!pid){ //if child
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            if(i==0){
                sprintf(args,"%d %d",wd_server[0], wd_server[1]);
                argsServer[3]=args;
                summon(argsServer);

            }else if(i==1){ //WINDOW
                sprintf(args,"%d %d|%d %d",window_keyboard[0],window_keyboard[1], wd_window[0], wd_window[1]); // pass fds to the argument
                argsWindow[3]=args;
                summon(argsWindow);

            }else if(i==2){ //KEYBOARD MANAGER
                sprintf(args,"%d %d|%d %d|%d %d", window_keyboard[0],window_keyboard[1],keyboard_drone[0],keyboard_drone[1], wd_keyboard[0], wd_keyboard[1]);
                argsKeyboard[3]=args;
                summon(argsKeyboard);

            }else if(i==3){ //DRONE
                sprintf(args,"%d %d|%d %d",keyboard_drone[0],keyboard_drone[1], wd_drone[0],wd_drone[1]);
                argsDrone[3]=args;
                summon(argsDrone);

            }else{ //WATCH DOG
                sprintf(args, "%d %d|%d %d|%d %d|%d %d|%d", wd_server[0],wd_server[1], wd_window[0], wd_window[1], wd_keyboard[0],wd_keyboard[1], wd_drone[0], wd_drone[1],all_pid[2]);
                argsWatchdog[3]=args;
                summon(argsWatchdog);
            }   
        }else { //else if parent
            printf("Summoned child with pid %d\n", pid);
        }
    }
    // terminate if any of the children are terminated
    int stat;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid_t pid = wait(&stat);
        printf("Child %d terminated with status %d\n", pid, stat);
    }
    return EXIT_SUCCESS;
}
    
    


