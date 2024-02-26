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
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/utility.c"
#include "../include/constants.h"
#include "../include/log.c"
#include <stdbool.h>
struct shared_data data, updated_data;
float drone_pos[6];// Array to store the position of drone
float obst_pos[NUM_OBSTACLES*2];
float target_pos[NUM_TARGETS*2];
int key; 
int windowx=0;
int windowy=0;


// Signal handler for watchdog
void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        writeToLogFile(serverlogpath,"killed");
        exit(1);
        
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
        writeToLogFile(serverlogpath,"signal received");
    }
}




int main(int argc, char *argv[]) 
{   
    // Delete everything written in logfile
    clearLogFile(serverlogpath);
    

    // SIGNALS FOR THE WATCHDOG
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGUSR1, &sig_act, NULL);


    ////////////////////////////////////////////////////
    /* INITIALIZATION OF LOCAL VARIABLES*/
    ////////////////////////////////////////////////////
    
    // Initialize the variables and copy it to shared data
    int command_force[2]={0,0};
    memcpy(data.command_force, command_force, sizeof(command_force));
    struct timeval timeout2;
    timeout2.tv_sec = 0;  // Wait for 5 seconds
    timeout2.tv_usec = 0;


    ////////////////////////////////////////////////////
    /* INITIALIZATION OF PIPES*/
    ////////////////////////////////////////////////////
    int server_window[2];
    int server_keyboard[2];
    int server_drone[2];
    int server_wd[2];

    int rec_pipes[NUM_PROCESSES-1][2];

    sscanf(argv[1],server_format,   &rec_pipes[0][0],   &rec_pipes[0][1], &server_window[0],   &server_window[1],
                                    &rec_pipes[1][0],   &rec_pipes[1][1], &server_keyboard[0], &server_keyboard[1],
                                    &rec_pipes[2][0],   &rec_pipes[2][1], &server_drone[0],    &server_drone[1],
                                    &rec_pipes[3][0],   &rec_pipes[3][1], &server_wd[0],       &server_wd[1]); // Get the fds of the pipe to watchdog
    
    close(server_drone[0]); // Close unnecessary pipes
    close(server_keyboard[0]);
    close(server_window[0]);
    close(server_wd[0]);
    for(int i=0; i< NUM_PROCESSES-1; i++){
        close(rec_pipes[i][1]);
    }
    

    ///////////////////////////////////////////
    /* SENDING THE PID TO WATCHDOG*/
    ///////////////////////////////////////////
    pid_t server_pid,wd_pid;
    server_pid=getpid();

    //Read all pids of children processes using select
    pid_t all_pids[NUM_PROCESSES-2];
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
            writeToLogFile(serverlogpath, "SERVER: Pid recieved");
            printf("%d\n",all_pids[j]);
        }
    }

    // Get pid of itself and log it
    all_pids[NUM_PROCESSES-2] = getpid();
    char logMessage[256];
    sprintf(logMessage, "PID = %d\n",server_pid);
    writeToLogFile(serverlogpath, logMessage);

    // Send all pids to watchdog
    my_write(server_wd[1],all_pids,sizeof(all_pids),sizeof(all_pids));
    writeToLogFile(serverlogpath, "SERVER: Pid sent to watchdog");




    ///////////////////////////////////////////
    /* SOCKET CONNECTION*/
    ///////////////////////////////////////////
    // local variables for sockets
    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[MSG_LEN];

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Initialize sockfd
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
    }
    listen(sockfd,5);
    writeToLogFile(serverlogpath, "SERVER:  Listening");
    clilen = sizeof(cli_addr);



    int target_sockfd = 0;
    int obstacle_sockfd=0;
    while(target_sockfd == 0 || obstacle_sockfd == 0){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {perror("ERROR on accept");}

        char socket_msg[MSG_LEN];
        read_then_echo(newsockfd, socket_msg);

        if (socket_msg[0] == 'O'){obstacle_sockfd = newsockfd;}
        else if(socket_msg[0] == 'T'){target_sockfd = newsockfd;}
        else {close(newsockfd);}
    }

    // Variable: To compare current and previous data
    char prev_drone_msg[MSG_LEN] = "";

    bool window_size_sent;
    window_size_sent = false;
    while(1){
        ///////////////////////////////////////////
        /* COMMUNICATION WITHIN SERVER USING PIPES*/
        ///////////////////////////////////////////

        fd_set reading2;
        FD_ZERO(&reading2);

        for(int i=0; i<NUM_PROCESSES-1;i++){
            FD_SET(rec_pipes[i][0], &reading2);
        }
        for (int i = 0; i < NUM_PROCESSES-1; i++) {
            if (rec_pipes[i][0] > max_pipe_fd) {
                max_pipe_fd = rec_pipes[i][0];
            }
        }
        int ret_val= 0;
        ret_val = select(max_pipe_fd, &reading2, NULL, NULL, &timeout2);
        for(int j=0; j<(NUM_PROCESSES-2); j++){
            if(ret_val>0){

                if(FD_ISSET(rec_pipes[j][0],&reading2)){ // Only from pipes that are updated
                    my_read(rec_pipes[j][0],&updated_data,rec_pipes[j][1], sizeof(data)); 

                    switch (j){
                    case 0: //window
                        writeToLogFile(serverlogpath, "Window: User input received");
                        key=updated_data.key; // Only copy the updated variables to local 
                        data.key=updated_data.key; // Update shared data with the updated variables
                        data.windowx=updated_data.windowx;
                        data.windowy=updated_data.windowy;
                        windowx=data.windowx;
                        windowy=data.windowy;

                        my_write(server_keyboard[1],&data,server_keyboard[0],sizeof(data));
                        
                        break;
                    case 1: //keyboard
                        writeToLogFile(serverlogpath, "Keyboard: Command force received from keyboard");
                        memcpy(command_force, updated_data.command_force, sizeof(updated_data.command_force)); // Store it as local variable
                        memcpy(data.command_force, updated_data.command_force, sizeof(updated_data.command_force)); //Copy it to the shared data
                        my_write(server_drone[1],&data,server_drone[0],sizeof(data)); // Send the shared data to drone
                        break;

                    case 2: //drone
                        writeToLogFile(serverlogpath, "Drone: New drone_pos received from drone");
                        memcpy(drone_pos, updated_data.drone_pos, sizeof(updated_data.drone_pos));
                        memcpy(data.drone_pos, updated_data.drone_pos, sizeof(updated_data.drone_pos));
                        memcpy(target_pos, updated_data.target_pos, sizeof(updated_data.target_pos));
                        memcpy(data.target_pos, updated_data.target_pos, sizeof(updated_data.target_pos));
                        my_write(server_window[1],&data,server_window[0],sizeof(data)); 
                        break;

                    default:
                        break;
                    }
                }
            }
        }
        

        if(window_size_sent==false && windowx != 0 ){
            sprintf(logMessage, "Window size%d %d", data.windowx, data.windowy);
            writeToLogFile(serverlogpath,logMessage);
            char window_msg[MAX_MSG_LEN];
            sprintf(window_msg, "%d %d", data.windowx, data.windowy);
            write(target_sockfd, window_msg, strlen(window_msg));
            write(obstacle_sockfd, window_msg, strlen(window_msg));
            writeToLogFile(serverlogpath,"WROTE WINDOW SIZE TO CLIENT");
            window_size_sent=true;
        }

        // if space bar is pressed to exit
        if(command_force[0] == -100 && command_force[1] == -100){
            sprintf(logMessage, "command forces %d %d", data.command_force[0], data.command_force[1]);
            writeToLogFile(serverlogpath, logMessage);
            char stop[5];
            sprintf(stop, "STOP");
            write(target_sockfd, stop, strlen(stop));
            write(obstacle_sockfd, stop, strlen(stop));
            sleep(3);
            // checking
            // writeToLogFile(serverlogpath, stop);

            // write_then_wait_echo(sockfd, stop, sizeof(stop));
            // // checks
            // sprintf(logMessage, "GAME STOP sent");
            // writeToLogFile(serverlogpath, logMessage);
        }

        // send GE if all target pos are 0

        // this version doesn't check if they're all zeors
        // for(int e = 0; e < NUM_TARGETS*2; e++){
        //     if(target_pos[e] == 0){
        //         char game_end[MSG_LEN];
        //         sprintf(game_end, "GE");
        //         write(target_sockfd, game_end, strlen(game_end));
        //         writeToLogFile(serverlogpath, "GAME END NEW TARGETS REQUESTED");
        //     }
        // }

        // send GE if all target pos are 0
        bool all_targets_zero = true;
        for(int e = 0; e < NUM_TARGETS*2; e++){
            if(target_pos[e] != 0){
                all_targets_zero =false;
                break;
            }
        }
        if (all_targets_zero){
            char game_end[MSG_LEN];
            sprintf(game_end, "GE");
            write(target_sockfd, game_end, strlen(game_end));
            writeToLogFile(serverlogpath, "GAME END NEW TARGETS REQUESTED");
        }


        //////////////////////////////////////////////////////
        /* Handle socket from targets.c */
        /////////////////////////////////////////////////////
        char targets_msg[MSG_LEN];
        
        if (read_then_echo_unblocked(target_sockfd, targets_msg,data.windowx,data.windowy) == 1){
            writeToLogFile(serverlogpath,targets_msg);
            int totalTargets;
            int index = 0;
            sscanf(targets_msg, "T[%d]", &totalTargets);
            // float temp_pos[totalTargets*2];

            char *token = strtok(targets_msg + 5, "|");
            while (token != NULL && index < totalTargets*2) {
                sscanf(token, "%f,%f", &target_pos[index*2], &target_pos[index*2+1]);
                token = strtok(NULL, "|");
                index++;
            }
            memcpy(data.target_pos, target_pos, sizeof(target_pos));
            sprintf(logMessage, "T:%f %f O:%f %f", data.target_pos[0],data.target_pos[1],data.obst_pos[0],data.obst_pos[1]);
            writeToLogFile(serverlogpath,logMessage);
            my_write(server_drone[1],&data,server_drone[0],sizeof(data));
            fflush(stdout);
        }

        
        ///////////////////////////////////////////
        /* COMMUNICATION WITH CLIENT USING SOCKETS */
        ///////////////////////////////////////////
        char obstacles_msg[MSG_LEN];
        if (read_then_echo_unblocked(obstacle_sockfd, obstacles_msg,data.windowx,data.windowy) == 1){
            writeToLogFile(serverlogpath,obstacles_msg);
            int totalObstacles;
            int index = 0;
            sscanf(obstacles_msg, "O[%d]", &totalObstacles);

            char *token = strtok(obstacles_msg + 5, "|");
            while (token != NULL && index < totalObstacles*2) {
                sscanf(token, "%f,%f", &obst_pos[index*2], &obst_pos[index*2+1]);
                token = strtok(NULL, "|");
                index++;
            }
            memcpy(data.obst_pos, obst_pos, sizeof(obst_pos));
            sprintf(logMessage, "T:%f %f O:%f %f", data.target_pos[0],data.target_pos[1],data.obst_pos[0],data.obst_pos[1]);
            writeToLogFile(serverlogpath,logMessage);
            my_write(server_drone[1],&data,server_drone[0],sizeof(data));
            fflush(stdout);
        }

        


        if (newsockfd < 0) 
            perror("ERROR on accept");

        
    }


    // clean up
    // close(server_drone[1]);
    // close(server_keyboard[1]);
    // close(server_window[1]);
    // close(server_wd[1]);
    // for(int i=0; i< NUM_PROCESSES-1; i++){
    //     close(rec_pipes[i][0]);
    // }

    return 0; 
} 
