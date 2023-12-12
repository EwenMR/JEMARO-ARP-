#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <unistd.h> 
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/select.h>
#include "../include/utility.c"
#include "../include/constants.h"

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
    printf("HELLO\n");
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

    int send_pipes[NUM_PROCESSES-1][2];
    int rec_pipes[NUM_PROCESSES-1][2];


    char server_format[100]= "%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d";
    
    // sscanf(argv[1],server_format,   &window_server[0],   &window_server[1],   &server_window[0],   &server_window[1],
    //                                 &keyboard_server[0], &keyboard_server[1], &server_keyboard[0], &server_keyboard[1],
    //                                 &drone_server[0],    &drone_server[1],    &server_drone[0],    &server_drone[1],
    //                                 &obstacle_server[0], &obstacle_server[1], &server_obstacle[0], &server_obstacle[1],
    //                                 &target_server[0],   &target_server[1],   &server_target[0],   &server_target[1],
    //                                 &wd_server[0], &wd_server[1], &server_wd[0], &server_wd[1]); // Get the fds of the pipe to watchdog
    
    sscanf(argv[1],server_format,   &rec_pipes[0][0],   &rec_pipes[0][1], &server_window[0],   &server_window[1],
                                    &rec_pipes[1][0],   &rec_pipes[1][1], &server_keyboard[0], &server_keyboard[1],
                                    &rec_pipes[2][0],   &rec_pipes[2][1], &server_drone[0],    &server_drone[1],
                                    &rec_pipes[3][0],   &rec_pipes[3][1], &server_obstacle[0], &server_obstacle[1],
                                    &rec_pipes[4][0],   &rec_pipes[4][1], &server_target[0],   &server_target[1],
                                    &rec_pipes[5][0],   &rec_pipes[5][1], &server_wd[0],       &server_wd[1]); // Get the fds of the pipe to watchdog
    
    
    
    // printf(server_format,window_server[0],   window_server[1],   server_window[0],   server_window[1],
    //                                         keyboard_server[0], keyboard_server[1], server_keyboard[0], server_keyboard[1],
    //                                         drone_server[0],    drone_server[1],    server_drone[0],    server_drone[1],
    //                                         obstacle_server[0], obstacle_server[1], server_obstacle[0], server_obstacle[1],
    //                                         target_server[0],   target_server[1],   server_target[0],   server_target[1],
    //                                         wd_server[0], wd_server[1], server_wd[0], server_wd[1]);
    close(server_drone[0]);
    close(server_keyboard[0]);
    close(server_window[0]);
    close(server_obstacle[0]);
    close(server_target[0]);
    close(server_wd[0]);



    double position[6]; // Array to store the position of drone
    pid_t all_pids[NUM_PROCESSES];
    
    fd_set reading;


    // while(1){
        FD_ZERO(&reading);
        for(int i=0; i<(NUM_PROCESSES-2);i++){
            FD_SET(rec_pipes[i][0], &reading);
        }
        int max_pipe_fd = -1;
        for (int i = 0; i < (NUM_PROCESSES-2); ++i) {
            if (rec_pipes[i][0] > max_pipe_fd) {
                max_pipe_fd = rec_pipes[i][0];
            }
        }
        int ret_val = select(max_pipe_fd, &reading, NULL, NULL, NULL);
        // printf("%d\n",ret_val);

        for(int j=0; j<(NUM_PROCESSES-2); j++){
            if(ret_val>0){
                FD_ISSET(rec_pipes[j][0],&reading);
                my_read(rec_pipes[j][0],&all_pids[j],rec_pipes[j][1], sizeof(int));
                printf("%d\n",all_pids[j]);
            }
            
        }
        
    // my_write(server_wd[1],all_pids,server_wd[0]);  
    all_pids[NUM_PROCESSES-2] = getpid();
    my_write(server_wd[1],all_pids,sizeof(all_pids),sizeof(all_pids));
    // }
    while(1){

    }

    
    //reset reading set


    





    
    int key, first,force[2];
    first =0;

    // while (1) 
    // {      
    //     // copy the position of the drone
    //     //USE SELECT
    //     if (first==0){
    //         my_read(window_server[0],position, server_window[1]);
    //         my_write(server_drone[1], position, drone_server[0]);
    //         //ALSO SEND IT TO OBST TARGET
    //     }
    //     // RECEIVE OBST TARGET
    //     // SEND OBST TARGET TO WINDOW

    //     my_read(window_server[0],&key,server_window[1]);
    //     my_write(server_keyboard[1],&key, keyboard_server[0]);

    //     my_read(keyboard_server[0],force, server_keyboard[1]);
    //     my_write(server_drone[1],force, drone_server[0]);

    //     my_read(drone_server[0],position,server_drone[1]);
    //     my_write(server_window[1],position, window_server[0]);
    //     //ALSO SEND POSITION TO TARGET AND OBSTACLE


    // } 

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

    // close(window_server[1]);
    // close(drone_server[1]);
    // close(keyboard_server[1]);
    // close(target_server[1]);
    // close(obstacle_server[1]);
    // close(wd_server[1]);

    return 0; 
} 