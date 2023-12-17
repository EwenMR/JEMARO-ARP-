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
#include <sys/time.h>
#include <time.h>
#include "../include/utility.c"
#include "../include/constants.h"
#include "../include/log.c"


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
    // Delete everything written in logfile
    clearLogFile(logpath);

    // SIGNALS FOR THE WATCHDOG
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGUSR1, &sig_act, NULL);

    
    // SENDING THE PID TO WATCHDOG
    pid_t server_pid,wd_pid;
    server_pid=getpid();

    int server_window[2];
    int server_keyboard[2];
    int server_drone[2];
    int server_obstacle[2];
    int server_target[2];
    int server_wd[2];

    int rec_pipes[NUM_PROCESSES-1][2];
    //Rec_pipes in order of
    /*
    WINDOW
    KEYBOARD
    DRONE
    OBSTACLE
    TARGET
    WD
    */

    sscanf(argv[1],server_format,   &rec_pipes[0][0],   &rec_pipes[0][1], &server_window[0],   &server_window[1],
                                    &rec_pipes[1][0],   &rec_pipes[1][1], &server_keyboard[0], &server_keyboard[1],
                                    &rec_pipes[2][0],   &rec_pipes[2][1], &server_drone[0],    &server_drone[1],
                                    &rec_pipes[3][0],   &rec_pipes[3][1], &server_obstacle[0], &server_obstacle[1],
                                    &rec_pipes[4][0],   &rec_pipes[4][1], &server_target[0],   &server_target[1],
                                    &rec_pipes[5][0],   &rec_pipes[5][1], &server_wd[0],       &server_wd[1]); // Get the fds of the pipe to watchdog
    
    close(server_drone[0]); // Close unnecessary pipes
    close(server_keyboard[0]);
    close(server_window[0]);
    close(server_obstacle[0]);
    close(server_target[0]);
    close(server_wd[0]);
    for(int i=0; i< NUM_PROCESSES-1; i++){
        close(rec_pipes[i][1]);
    }
    

    //PIDS FOR WATCHDOG USING SELECT
    pid_t all_pids[NUM_PROCESSES];
    fd_set reading;
   
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
    for(int j=0; j<(NUM_PROCESSES-2); j++){
        if(ret_val>0){
            FD_ISSET(rec_pipes[j][0],&reading);
            my_read(rec_pipes[j][0],&all_pids[j],rec_pipes[j][1], sizeof(int));
            writeToLogFile(logpath, "SERVER: Pid recieved");
            printf("%d\n",all_pids[j]);
        }
    }
    all_pids[NUM_PROCESSES-2] = getpid();
    my_write(server_wd[1],all_pids,sizeof(all_pids),sizeof(all_pids));
    writeToLogFile(logpath, "SERVER: Pid sent to watchdog");
    


    // Local variables
    struct shared_data data, updated_data;
    double drone_pos[6];// Array to store the position of drone
    double obst_pos[NUM_OBSTACLES*2];
    double target_pos[NUM_TARGETS*2];
    int key;

    // Initialize the variables and copy it to shared data
    int command_force[2]={0,0};
    memcpy(data.command_force, command_force, sizeof(command_force));
    

    while(1){
        // SELECT
        fd_set reading;
        FD_ZERO(&reading);

        for(int i=0; i<6;i++){
            FD_SET(rec_pipes[i][0], &reading);
        }
        for (int i = 0; i < 6; i++) {
            if (rec_pipes[i][0] > max_pipe_fd) {
                max_pipe_fd = rec_pipes[i][0];
            }
        }
        int ret_val= 0;
        ret_val = select(max_pipe_fd, &reading, NULL, NULL, NULL);
        for(int j=0; j<(NUM_PROCESSES-2); j++){
            if(ret_val>0){

                if(FD_ISSET(rec_pipes[j][0],&reading)){ // Only from pipes that are updated
                    my_read(rec_pipes[j][0],&updated_data,rec_pipes[j][1], sizeof(data)); 

                    switch (j){
                    case 0: //window
                        writeToLogFile(logpath, "SERVER: User input received");
                
                        key=updated_data.key; // Only copy the updated variables to local 
                        data.key=updated_data.key; // Update shared data with the updated variables
                        my_write(server_keyboard[1],&data,server_keyboard[0],sizeof(data));
                        
                        break;
                    case 1: //keyboard
                        writeToLogFile(logpath, "SERVER: Command force received from keyboard");
                        memcpy(command_force, updated_data.command_force, sizeof(updated_data.command_force));
                        memcpy(data.command_force, updated_data.command_force, sizeof(updated_data.command_force));

                        my_write(server_drone[1],&data,server_drone[0],sizeof(data));
                        break;
                    case 2: //drone
                        writeToLogFile(logpath, "SERVER: New drone_pos received from drone");
                        memcpy(drone_pos, updated_data.drone_pos, sizeof(updated_data.drone_pos));
                        memcpy(data.drone_pos, updated_data.drone_pos, sizeof(updated_data.drone_pos));

                        my_write(server_target[1],&data,server_target[0],sizeof(data));
                        my_write(server_window[1],&data,server_window[0],sizeof(data));
                        break;
                    case 3: //obstacle
                        writeToLogFile(logpath, "SERVER: Obstacle received from obstacle");
                        memcpy(obst_pos, updated_data.obst_pos, sizeof(updated_data.obst_pos));
                        memcpy(data.obst_pos, updated_data.obst_pos, sizeof(updated_data.obst_pos));

                        my_write(server_drone[1],&data,server_drone[0],sizeof(data));
                        break;
                    case 4: //target
                        writeToLogFile(logpath, "SERVER: Target received from target");
                        memcpy(target_pos, updated_data.target_pos, sizeof(updated_data.target_pos));
                        memcpy(data.target_pos, updated_data.target_pos, sizeof(updated_data.target_pos));
                        

                        my_write(server_obstacle[1],&data,rec_pipes[3][0],sizeof(data));
                        break;
                    default:
                        break;
                    }
                }
            }
        }

    }



    // clean up
    close(server_drone[1]);
    close(server_keyboard[1]);
    close(server_window[1]);
    close(server_obstacle[1]);
    close(server_target[1]);
    close(server_wd[1]);
    for(int i=0; i< NUM_PROCESSES-1; i++){
        close(rec_pipes[i][0]);
    }

    return 0; 
} 