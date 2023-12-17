
// window.c
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "../include/constants.h"
#include "../include/utility.c"
#include <signal.h>
#include "../include/log.c"

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
    }
}

double calc_drone_pos(double force,double x1,double x2){
    double x;
    x= (force*T*T-M*x2+2*M*x1+K*T*x1)/(M+K*T); //Eulers method

    // Dont let it go outside of the window
    if(x<0){
        return 0;
    }else if(x>BOARD_SIZE){
        return BOARD_SIZE;
    }

    return x;
}

// Get the new drone_pos using calc_function and store the previous drone_poss
double update_pos(double* drone_pos, int* xy){
    double new_posx,new_posy;
    new_posx=calc_drone_pos(xy[0],drone_pos[4],drone_pos[2]);
    new_posy=calc_drone_pos(xy[1],drone_pos[5],drone_pos[3]);

    // update drone_pos
    for(int i=0; i<4; i++){
        drone_pos[i]=drone_pos[i+2];
    }
    drone_pos[4]=new_posx;
    drone_pos[5]=new_posy;
    return *drone_pos;
}

double stop(double *drone_pos){
    for(int i=0; i<4; i++){
        drone_pos[i]=drone_pos[i+2];}
    return *drone_pos;
}



int main(int argc, char *argv[]) {

    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    // VARIABLES
    struct shared_data data;
    int xy[2]; // xy = force direction of x,y as such -> [0,1]
    double drone_pos[6]={BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2};
    

    // PIPES
    int drone_server[2], server_drone[2];
    sscanf(argv[1], args_format,  &drone_server[0], &drone_server[1], &server_drone[0], &server_drone[1]);
    close(drone_server[0]); //Close unnecessary pipes
    close(server_drone[1]);


    // PIDS FOR WATCHDOG
    pid_t drone_pid;
    drone_pid=getpid();
    my_write(drone_server[1], &drone_pid, server_drone[0],sizeof(drone_pid));
    writeToLogFile(logpath, "DRONE: Pid sent to server");


    // Send initial drone position to other processes
    memcpy(data.drone_pos,drone_pos, sizeof(drone_pos));
    my_write(drone_server[1],&data, server_drone[0],sizeof(data));
    writeToLogFile(logpath, "DRONE: Initial drone_pos sent to server");


    while (1) {
        // 3 Receive command force
        my_read(server_drone[0], &data, drone_server[1], sizeof(data));
        memcpy(xy, data.command_force, sizeof(data.command_force));
        writeToLogFile(logpath, "DRONE: Command_force received from server");

        if(xy[0]==0 && xy[1]==0){
            stop(drone_pos);
        }else{
            update_pos(drone_pos,xy); 
        }

        // Send updated drone drone_pos to window and target
        memcpy(data.drone_pos, drone_pos, sizeof(drone_pos));
        my_write(drone_server[1], &data, server_drone[0],sizeof(data));
        writeToLogFile(logpath, "DRONE: Drone_pos sent to server");
    }

    // Clean up
    close(server_drone[0]);
    close(drone_server[1]);
    

    return 0;
}
