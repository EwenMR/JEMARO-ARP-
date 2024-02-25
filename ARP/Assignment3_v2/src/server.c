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
struct shared_data data, updated_data;
float drone_pos[6];// Array to store the position of drone
float obst_pos[NUM_OBSTACLES*2];
float target_pos[NUM_TARGETS*2];
int key; 
    

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

        //////////////////////////////////////////////////////
        /* Handle socket from targets.c */
        /////////////////////////////////////////////////////
        char targets_msg[MSG_LEN];
        if (read_then_echo_unblocked(target_sockfd, targets_msg) == 1){
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
        if (read_then_echo_unblocked(obstacle_sockfd, obstacles_msg) == 1){
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


        // for (int i = 0; i < FD_SETSIZE; ++i) {
        //     if (FD_ISSET(i, &read_fds)) {
        //         if (i == sockfd) {  // New connection
        //             clilen = sizeof(cli_addr);
        //             newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        //             if (newsockfd < 0) {
        //                 perror("ERROR on accept");
        //                 exit(1);
        //             }
        //             FD_SET(newsockfd, &read_fds);
        //             writeToLogFile(serverlogpath,"New connection established\n");
                    

        //         } else {  // Existing client
        //             int bytes_read = read(i, buffer, sizeof(buffer) - 1);
        //             writeToLogFile(serverlogpath,buffer);

        //             //Echo
        //             int bytes_written = write(i,buffer, bytes_read);
        //             if (bytes_written < 0) {perror("ERROR writing to socket");}
                    
                    
        //             if (bytes_read < 0) {
        //                 perror("ERROR reading from socket");
        //                 exit(1);
        //             } else if (bytes_read == 0) { // Connection closed
        //                 close(i);
        //                 FD_CLR(i, &read_fds);
        //                 printf("Connection closed\n");
        //             } else { // Message received
        //                 buffer[bytes_read] = '\0';
        //                 writeToLogFile(serverlogpath,buffer);


        //                 if (buffer[0]=='O' && buffer[1]=='I'){//identification done
        //                     //SEND BACK SCREEN SIZE
        //                     writeToLogFile(serverlogpath,"OI RECEIVED");
        //                 }else if(buffer[0]=='T' && buffer[1]=='I'){
        //                     writeToLogFile(serverlogpath,"TI RECEIVED");//identification done
        //                     //SEND BACK SCREEN SIZE
        //                 }
        //                 if(buffer[0]=='O'&&buffer[1]!='I'){//obstacle positions sent
        //                     writeToLogFile(serverlogpath,"Obstacle received");
        //                     //PARSE THE DATA TO STRING
        //                     int totalObstacles;
        //                     int index = 0;
        //                     sscanf(buffer, "O[%d]", &totalObstacles);
        //                     float temp_pos[totalObstacles*2];

        //                     char *token = strtok(buffer + 5, "|");
        //                     while (token != NULL && index < totalObstacles*2) {
        //                         sscanf(token, "%f,%f", &temp_pos[index*2], &temp_pos[index*2+1]);
        //                         // data.obst_pos[index] = atof(token);
        //                         token = strtok(NULL, "|");
        //                         index++;
                                
        //                     }
        //                     memcpy(data.obst_pos, temp_pos, sizeof(temp_pos));
        //                     my_write(server_drone[1],&data,server_drone[0],sizeof(data));
        //                 }
        //                 if(buffer[0]=='T' && buffer[1]!='I'){//target positions sent
        //                     writeToLogFile(serverlogpath,"Target received");
        //                     //PARSE THE DATA TO STRING
        //                     int totalTargets;
        //                     int index = 0;
        //                     sscanf(buffer, "T[%d]", &totalTargets);
        //                     float temp_pos[totalTargets*2];

        //                     char *token = strtok(buffer + 5, "|");
        //                     while (token != NULL && index < totalTargets*2) {
        //                         sscanf(token, "%f,%f", &temp_pos[index*2], &temp_pos[index*2+1]);
        //                         // temp_pos[index] = atof(token);
        //                         token = strtok(NULL, "|");
        //                         index++;
        //                     }

        //                     writeToLogFile(serverlogpath,buffer);
        //                     memcpy(data.target_pos, temp_pos, sizeof(temp_pos));
        //                     my_write(server_drone[1],&data,server_drone[0],sizeof(data));


        //                 }
                        
                        
                        



        //             }
        //         }
        //     }
            // writeToLogFile(serverlogpath, "NO FD_ISSET\n");