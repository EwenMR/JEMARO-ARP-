//master.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/constants.h"


void summon(char **programArgs){
    execvp(programArgs[0], programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    // arguments to summon child konsole
    char *argsServer[] = {"konsole", "-e","./build/server", "placeholder",NULL};
    char *argsWindow[] = {"konsole", "-e", "./build/window", "placeholder",NULL};
    char *argsDrone[] = { "konsole", "-e","./build/drone", "placeholder",NULL};
    char *argsKeyboard[] = {"konsole", "-e","./build/keyboard", "placeholder",NULL};
    char *argsObstacle[] = {"./build/obstacle", "placeholder", NULL};
    char *argsTarget[] =  {"./build/target", "placeholder", NULL};
    char *argsWatchdog[] = {"./build/watchdog", "placeholder",NULL};

    // PIPES
    int window_server[2];
    int server_window[2];

    int keyboard_server[2];
    int server_keyboard[2];

    int drone_server[2];
    int server_drone[2];

    int obstacle_server[2];
    int server_obstacle[2];

    int target_server[2];
    int server_target[2];

    int wd_server[2];
    int server_wd[2];

    char server_format[100]= "%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d";
    char args_format[80]="%d %d|%d %d";

    if (pipe(window_server)   == -1 ||  pipe(server_window)   == -1 ||
        pipe(keyboard_server) == -1 ||  pipe(server_keyboard) == -1 ||
        pipe(drone_server)    == -1 ||  pipe(server_drone)    == -1 ||
        pipe(obstacle_server) == -1 ||  pipe(server_obstacle) == -1 ||
        pipe(target_server)   == -1 ||  pipe(server_target)   == -1 ||
        pipe(wd_server) == -1 ||  pipe(server_wd) == -1 ) {
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
                sprintf(args,server_format, window_server[0],   window_server[1],   server_window[0],   server_window[1],
                                            keyboard_server[0], keyboard_server[1], server_keyboard[0], server_keyboard[1],
                                            drone_server[0],    drone_server[1],    server_drone[0],    server_drone[1],
                                            obstacle_server[0], obstacle_server[1], server_obstacle[0], server_obstacle[1],
                                            target_server[0],   target_server[1],   server_target[0],   server_target[1],
                                            wd_server[0], wd_server[1], server_wd[0], server_wd[1]);
                argsServer[3]=args;
                summon(argsServer);

            }else if(i==1){ //WINDOW
                sprintf(args, args_format,  window_server[0],   window_server[1],   server_window[0],   server_window[1]); // pass fds to the argument
                argsWindow[3]=args;
                summon(argsWindow);

            }else if(i==2){ //KEYBOARD MANAGER
                sprintf(args, args_format,  keyboard_server[0], keyboard_server[1], server_keyboard[0], server_keyboard[1]);
                argsKeyboard[3]=args;
                summon(argsKeyboard);

            }else if(i==3){ //DRONE
                sprintf(args, args_format,  drone_server[0],    drone_server[1],    server_drone[0],    server_drone[1]);
                argsDrone[3]=args;
                summon(argsDrone);

            }else if(i==4){ //OBSTACLE
                sprintf(args, args_format,  obstacle_server[0], obstacle_server[1], server_obstacle[0], server_obstacle[1]);
                argsObstacle[1]=args;
                summon(argsObstacle);

            }else if(i==5){ //TARGET
                sprintf(args, args_format,  target_server[0],   target_server[1],   server_target[0],   server_target[1]);
                argsTarget[1]=args;
                summon(argsTarget);

            }else if(i==6){ //WATCH DOG
                sprintf(args, "%d %d", server_wd[0], server_wd[1]);
                argsWatchdog[1]=args;
                summon(argsWatchdog);
            }   
        }else { //else if parent
            printf(server_format, window_server[0],   window_server[1],   server_window[0],   server_window[1],
                                            keyboard_server[0], keyboard_server[1], server_keyboard[0], server_keyboard[1],
                                            drone_server[0],    drone_server[1],    server_drone[0],    server_drone[1],
                                            obstacle_server[0], obstacle_server[1], server_obstacle[0], server_obstacle[1],
                                            target_server[0],   target_server[1],   server_target[0],   server_target[1],
                                            wd_server[0], wd_server[1], server_wd[0], server_wd[1]);
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
    
    


