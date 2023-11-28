//master.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include "include/constants.h"

void summon(char **programArgs){
    execvp("konsole", programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    // arguments to summon child konsole
    char *argsServer[] = {"konsole",  "-e","./build/server", NULL};
    char *argsWindow[] = {"konsole",  "-e", "./build/window", "placeholder",NULL};
    char *argsDrone[] = {"konsole",  "-e", "./build/drone", "placeholder",NULL};
    char *argsKeyboard[] = {"konsole",  "-e","./build/keyboard", "placeholder",NULL};
    char *argsWatchdog[] = {"konsole",  "-e", "./build/watchdog", NULL};


    // pipes fd
    int window_keyboard[2];
    int keyboard_drone[2];
    if (pipe(window_keyboard) == -1 || pipe(keyboard_drone) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }


    for (int i=0; i<5; i++){
        // fork children
        pid_t pid = fork(); 
        char args[MAX_MSG_LEN];
        if(!pid){ //if child
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            if(i==0){
                summon(argsServer);
            }else if(i==1){ //WINDOW
                // pass pipes fds to the argument
                sprintf(args,"%d %d",window_keyboard[0],window_keyboard[1]);
                argsWindow[3]=args;
                summon(argsWindow);

            }else if(i==2){ //KEYBOARD MANAGER
                sprintf(args,"%d %d|%d %d",window_keyboard[0],window_keyboard[1],keyboard_drone[0],keyboard_drone[1]);
                argsKeyboard[3]=args;
                summon(argsKeyboard);

            }else if(i==3){ //DRONE
                sprintf(args,"%d %d",keyboard_drone[0],keyboard_drone[1]);
                argsDrone[3]=args;
                summon(argsDrone);

            }else{ //WATCH DOG
                summon(argsWatchdog);
            }   
        }else { //else if parent
            printf("Summoned child with pid %d\n", pid);
        }
    }
    // terminate if any of the children are terminated
    int stat;
    for (int i = 0; i < 5; i++) {
        pid_t pid = wait(&stat);
        printf("Child %d terminated with status %d\n", pid, stat);
    }
    return EXIT_SUCCESS;
}
    
    


