
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
#include <math.h>

#define p 5
#define n 70

// VARIABLES
struct shared_data data;
int xy[2]; // xy = force direction of x,y as such -> [0,1]
double drone_pos[6]={BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2,BOARD_SIZE/2};
double obst_pos[NUM_OBSTACLES*2], target_pos[NUM_TARGETS*2];
char logMessage[100]; 


char *drone_logpath = "./log/drone.log"; // Path for the log file

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
    }
}
// KOHEI'S ATTEMPT FAILED
double calculateDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

double calculateAngle(double x1, double y1, double x2, double y2) {
    double angleRadians = atan2(y2 - y1, x2 - x1);
    return angleRadians;
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









double stop(double *drone_pos){
    for(int i=0; i<4; i++){
        drone_pos[i]=drone_pos[i+2];}
    return *drone_pos;
}



double *calc_potential(double* obstacle_pos, double* drone_pos, int* xy){
    double sumx=0,sumy=0;
    
    for(int i = 0; i < NUM_OBSTACLES; i++){
        double p_q = sqrt(pow(obstacle_pos[2*i] - drone_pos[4], 2) + pow(obstacle_pos[2*i+1] - drone_pos[5],2));
        double p_wall[] = {drone_pos[4], drone_pos[5], BOARD_SIZE-drone_pos[4], BOARD_SIZE-drone_pos[5]};
        if(p_q <= p){
            sprintf(logMessage, "OBSTACLE x: %f, OBSTACLE y: %f, DRONE x: %f, DRONE y: %f,", obst_pos[i*2], obst_pos[i*2+1], drone_pos[4],drone_pos[5]);
            writeToLogFile(drone_logpath, logMessage);

            double angle= calculateAngle(drone_pos[4],drone_pos[5],obstacle_pos[i*2],obstacle_pos[i*2+1]);
            sumx += pow((1/p_q)-(1/p), 2)*cos(angle);
            sumy += pow((1/p_q)-(1/p), 2)*sin(angle);
        }

        

        
        // if(p_wall[0]<=p && xy[0]<0){
        //     sumx += pow((1/p_wall[0])-(1/p), 2);
        // }else if(p_wall[1] <= p && xy[1]<0){
        //     sumy += pow((1/p_wall[1])-(1/p), 2);
        // }else if(p_wall[2] <= p && xy[0] > 0){
        //     sumx += pow((1/p_wall[2])-(1/p), 2);
        // }else if(p_wall[3] <= p && xy[1] >0){
        //     sumy += pow((1/p_wall[3])-(1/p), 2);
        // }

        // for(int j=0; j<2; j++){
        //     if(p_wall[j]<=p){
        //         sumx += pow((1/p_wall[j])-(1/p), 2);
        //     }
        // for(int j=2; j<4; j++){
        //     if(p_wall[j]<=p){
        //         sumy += pow((1/p_wall[j])-(1/p), 2);
        //     }
        // }
            
        // }
    }
    double* rep = (double*)malloc(2 * sizeof(double));
    if (rep == NULL) {
        // Handle memory allocation failure
        exit(EXIT_FAILURE); // Or handle it in a way that suits your program
    }

    // double result[2];
    rep[0] = -0.5 * n * sumx;
    rep[1] = -0.5 * n * sumy;
    sprintf(logMessage, "REP x: %f, REP y: %f, Sumx: %f, Sumy: %f", rep[0], rep[1], sumx, sumy);
    writeToLogFile(drone_logpath, logMessage);
    return rep;
}


// Get the new drone_pos using calc_function and store the previous drone_poss
void update_pos(double* drone_pos,double* obstacle_pos, int* xy){
    double new_posx,new_posy;
    double repx,repy;
    repx = calc_potential(obstacle_pos, drone_pos,xy)[0];
    repy = calc_potential(obstacle_pos, drone_pos,xy)[1];
    // for(int i=0; i<NUM_OBSTACLES; i++){
    //     sprintf(logMessage, "OBSTACLE x: %f, OBSTACLE y: %f,", obst_pos[i*2], obst_pos[i*2+1]);
    //     writeToLogFile(drone_logpath, logMessage);
    // }
    

    sprintf(logMessage, "DRONE x: %f, DRONE y: %f, COMMAND x: %d, COMMAND y: %d, REP x: %f, REP y: %f", drone_pos[4],drone_pos[5],xy[0],xy[1],repx,repy);
    writeToLogFile(drone_logpath, logMessage);

    if(xy[0] > 0){
        new_posx = calc_drone_pos(xy[0] - repx, drone_pos[4], drone_pos[2]);
    } else if(xy[0] < 0){
        new_posx = calc_drone_pos(xy[0] + repx, drone_pos[4], drone_pos[2]);
    } else {
        new_posx = calc_drone_pos(xy[0], drone_pos[4], drone_pos[2]);
    }

    if(xy[1] > 0){
        new_posy = calc_drone_pos(xy[1] - repy, drone_pos[5], drone_pos[3]);
    } else if(xy[1] < 0){
        new_posy = calc_drone_pos(xy[1] + repy, drone_pos[5], drone_pos[3]);
    } else {
        new_posy = calc_drone_pos(xy[1], drone_pos[5], drone_pos[3]);
    } 

    // update drone_pos
    for(int i=0; i<4; i++){
        drone_pos[i]=drone_pos[i+2];
    }
    drone_pos[4]=new_posx;
    drone_pos[5]=new_posy;
    // return *drone_pos;
}




int main(int argc, char *argv[]) {
    clearLogFile(drone_logpath);

    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);



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
        memcpy(obst_pos, data.obst_pos, sizeof(data.obst_pos));
        writeToLogFile(logpath, "DRONE: Command_force received from server");

        if(xy[0]==0 && xy[1]==0){
            stop(drone_pos);
        }else{
            // update_pos(drone_pos,xy,obst_pos); 
            update_pos(drone_pos, obst_pos, xy);
        }

        // Send updated drone_pos to window and target
        memcpy(data.drone_pos, drone_pos, sizeof(drone_pos));
        my_write(drone_server[1], &data, server_drone[0],sizeof(data));
        writeToLogFile(logpath, "DRONE: Drone_pos sent to server");
    }

    // Clean up
    close(server_drone[0]);
    close(drone_server[1]);
    

    return 0;
}






// // Get the new drone_pos using calc_function and store the previous drone_poss
// double update_pos(double* drone_pos, int* xy, double obst_pos[]){
//     double new_posx,new_posy;
//     // new_posx=calc_drone_pos(xy[0],drone_pos[4],drone_pos[2]);
//     // new_posy=calc_drone_pos(xy[1],drone_pos[5],drone_pos[3]);

//     new_posx, new_posy = calc_drone_pos_withRep(xy,drone_pos[4],drone_pos[2], drone_pos[5],drone_pos[3], obst_pos);

//     // update drone_pos
//     for(int i=0; i<4; i++){
//         drone_pos[i]=drone_pos[i+2];
//     }
//     drone_pos[4]=new_posx;
//     drone_pos[5]=new_posy;
//     return *drone_pos;
// }



// double calc_repulsive_force(double drone_x, double drone_y, double obst_pos[]){
//     double dist, angle, sumx, sumy;
//     sumx = sumy= 0;
//     for (int i=0; i<NUM_OBSTACLES*2; i+=2){
//         dist= calculateDistance(drone_x, drone_y, obst_pos[i], obst_pos[i+1]);
//         if (dist > p){
//             return 0;
//         }else{
//             angle = calculateAngle(drone_x,drone_y,obst_pos[i],obst_pos[i+1]);
//             sumx += (pow((1/dist)-(1/p),2) * cos(angle));
//             sumy += (pow((1/dist)-(1/p),2) * sin(angle));
//         }
//     }
//     return -(1/2)*n*sumx, -(1/2)*n*sumy;
// }



// double calc_drone_pos_withRep(int force[], double drone_x1, double drone_x2, double drone_y1, double drone_y2, double obst_pos[]){
    
//     double repx, repy = calc_repulsive_force(drone_x1, drone_y1,obst_pos);
//     // double x = calc_drone_pos(force[0]-repx, drone_x1, drone_x2);
//     // double y = calc_drone_pos(force[1]-repy, drone_y1, drone_y2);
//     double x,y;
//     if(force[0] > 0){
//         x = calc_drone_pos(force[0] - repx, drone_x1, drone_x2);
//     } else if(force[0] < 0){
//         x = calc_drone_pos(force[0] + repx, drone_x1, drone_x2);
//     } else {
//         x = calc_drone_pos(force[0], drone_x1, drone_x2);
//     }

//     if(force[1] > 0){
//         y = calc_drone_pos(force[0] - repx, drone_y1, drone_y2);
//     } else if(force[1] < 0){
//         y = calc_drone_pos(force[0] + repx, drone_y1, drone_y2);
//     } else {
//         x = calc_drone_pos(force[0], drone_y1, drone_y2);
//     }
//     return x, y;
// }
