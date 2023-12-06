#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <unistd.h> 
#include <stdlib.h>
#include "../include/constants.h"
#include <semaphore.h>
#include <sys/mman.h>
#include "../include/utility.c"
#include <signal.h>

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

int main(int argc, char *argv[]) 
{
    // SIGNALS FOR THE WATCHDOG
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGUSR1, &sig_act, NULL);

    

    
    // SENDING THE PID TO WATCHDOG
    pid_t server_pid,wd_pid;
    server_pid=getpid();

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
    
    sscanf(argv[1],server_format,   &window_server[0],   &window_server[1],   &server_window[0],   &server_window[1],
                                    &keyboard_server[0], &keyboard_server[1], &server_keyboard[0], &server_keyboard[1],
                                    &drone_server[0],    &drone_server[1],    &server_drone[0],    &server_drone[1],
                                    &obstacle_server[0], &obstacle_server[1], &server_obstacle[0], &server_obstacle[1],
                                    &target_server[0],   &target_server[1],   &server_target[0],   &server_target[1],
                                    &wd_server[0], &wd_server[1], &server_wd[0], &server_wd[1]); // Get the fds of the pipe to watchdog
    printf(server_format,window_server[0],   window_server[1],   server_window[0],   server_window[1],
                                            keyboard_server[0], keyboard_server[1], server_keyboard[0], server_keyboard[1],
                                            drone_server[0],    drone_server[1],    server_drone[0],    server_drone[1],
                                            obstacle_server[0], obstacle_server[1], server_obstacle[0], server_obstacle[1],
                                            target_server[0],   target_server[1],   server_target[0],   server_target[1],
                                            wd_server[0], wd_server[1], server_wd[0], server_wd[1]);
    close(server_drone[0]);
    close(server_keyboard[0]);
    close(server_window[0]);
    close(server_obstacle[0]);
    close(server_target[0]);
    close(server_wd[0]);

    close(window_server[1]);
    close(drone_server[1]);
    close(keyboard_server[1]);
    close(target_server[1]);
    close(obstacle_server[1]);
    close(wd_server[1]);

    double position[6]; // Array to store the position of drone
    pid_t all_pids[NUM_PROCESSES];
    all_pids[0] = getpid();

    //USE SELECT?
    //read all pids
    printf("NOTHING READ\n");
    my_read(window_server[0],&all_pids[1],server_window[1]);
    printf("READ WINDOW\n");
    my_read(keyboard_server[0],&all_pids[2],server_keyboard[1]);
    printf("READ KEYBOARD\n");
    my_read(drone_server[0],&all_pids[3],server_drone[1]);
    printf("READ DRONE\n");
    my_read(obstacle_server[0],&all_pids[4],server_obstacle[1]);
    printf("READ OBSTACLE\n");
    my_read(target_server[0],&all_pids[5],server_target[1]);
    printf("READ TARGET\n");


    my_write(server_wd[1],all_pids,wd_server[0]);
    printf("WRITTEN WD\n");




    
    int key, first,force[2];
    first =0;

    while (1) 
    {      
        // copy the position of the drone
        //USE SELECT
        if (first==0){
            my_read(window_server[0],position, server_window[1]);
            my_write(server_drone[1], position, drone_server[0]);
            //ALSO SEND IT TO OBST TARGET
        }
        // RECEIVE OBST TARGET
        // SEND OBST TARGET TO WINDOW

        my_read(window_server[0],&key,server_window[1]);
        my_write(server_keyboard[1],&key, keyboard_server[0]);

        my_read(keyboard_server[0],force, server_keyboard[1]);
        my_write(server_drone[1],force, drone_server[0]);

        my_read(drone_server[0],position,server_drone[1]);
        my_write(server_window[1],position, window_server[0]);
        //ALSO SEND POSITION TO TARGET AND OBSTACLE


    } 

    // clean up
    close(server_drone[1]);
    close(server_keyboard[1]);
    close(server_window[1]);
    close(server_obstacle[1]);
    close(server_target[1]);
    close(server_wd[1]);

    close(window_server[0]);
    close(drone_server[0]);
    close(keyboard_server[0]);
    close(target_server[0]);
    close(obstacle_server[0]);
    close(wd_server[0]);

    return 0; 
} 