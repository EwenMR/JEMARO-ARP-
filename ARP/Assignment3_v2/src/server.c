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
    char test[50];

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
    

    //Read all pids of children processes using select
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
            writeToLogFile(serverlogpath, "SERVER: Pid recieved");
            printf("%d\n",all_pids[j]);
        }
    }

    // Get pid of itself and log it
    all_pids[NUM_PROCESSES-2] = getpid();
    char logMessage[80];
    sprintf(logMessage, "PID = %d\n",server_pid);
    writeToLogFile(serverlogpath, logMessage);

    // Send all pids to watchdog
    my_write(server_wd[1],all_pids,sizeof(all_pids),sizeof(all_pids));
    writeToLogFile(serverlogpath, "SERVER: Pid sent to watchdog");
    
    // Local variables
    struct shared_data data, updated_data;
    double drone_pos[6];// Array to store the position of drone
    double obst_pos[NUM_OBSTACLES*2];
    double target_pos[NUM_TARGETS*2];
    int key;

    // Initialize the variables and copy it to shared data
    int command_force[2]={0,0};
    memcpy(data.command_force, command_force, sizeof(command_force));

    //SOCKETS ------------------------------------------------------------

    int sockfd, newsockfd, portno, clilen, pid;
    struct sockaddr_in serv_addr, cli_addr;
    fd_set read_fds;
    char buffer[50];
    FD_ZERO(&read_fds);

    
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Initialize sockfd
    writeToLogFile(serverlogpath, "SERVER: Created socket instance");

    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Add sockfd to the read_fds set after it's initialized
    FD_SET(sockfd, &read_fds);  

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("Failed to get socket flags");
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Failed to set non-blocking mode");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    // portno = PORTNO;
    sprintf(logMessage, "port number = %d\n",portno);
    writeToLogFile(serverlogpath, logMessage);

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
    }
    listen(sockfd,5);
    writeToLogFile(serverlogpath, "SERVER:  Listening");
    clilen = sizeof(cli_addr);
    //------------------------------------------------------------
    

    while(1){

        /// TRYING TO TROUBLESHOOT RIGHT HERE
        struct timeval timeout;
        timeout.tv_sec = 5;  // Wait for 5 seconds
        timeout.tv_usec = 0;

        if (select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout) < 0) {
            // writeToLogFile(serverlogpath, "inside select");
            perror("ERROR in select");
            exit(1);
        }

        // UNTIL HERE

        // COMMUNICATION WITH CLIENT ---------------------------------------
        for (int i = 0; i < FD_SETSIZE; ++i) {
            // writeToLogFile(serverlogpath, "inside for loop");
            if (FD_ISSET(i, &read_fds)) {
                writeToLogFile(serverlogpath, "FD_ISSET\n");

                if (i == sockfd) {  // New connection
                    writeToLogFile(serverlogpath,"NEW CONNECTION\n");
                    clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                    if (newsockfd < 0) {
                        perror("ERROR on accept");
                        exit(1);
                    }
                    FD_SET(newsockfd, &read_fds);
                    writeToLogFile(serverlogpath,"New connection established\n");
                    

                } else {  // Existing client
                    writeToLogFile(serverlogpath, "Connected to client\n");
                    int bytes_read = read(i, buffer, sizeof(buffer) - 1);
                    if (bytes_read < 0) {
                        perror("ERROR reading from socket");
                        exit(1);
                    } else if (bytes_read == 0) { // Connection closed
                        close(i);
                        FD_CLR(i, &read_fds);
                        printf("Connection closed\n");
                    } else { // Message received
                        buffer[bytes_read] = '\0';

                        // // Code to check if data is sent from obstacle or target
                        // if (buffer[0] == 'O'){
                        //     // change the buffer into list
                        //     // store the data into obstacle[]
                        // }else if(buffer[0] == 'T'){
                        //     // change the buffer into list
                        //     // store the data into target[]
                        // }else{
                        //     perror("Client send the data in wrong format\n");
                        // }

                        // change string to list
                        // store it in target_pos

                        sprintf(logMessage, "target position: %s\n",buffer);
                        writeToLogFile(serverlogpath,logMessage);
                        // printf("Message received: %s\n", buffer);
                    }
                }
            }
            // writeToLogFile(serverlogpath, "NO FD_ISSET\n");
        }

        if (newsockfd < 0) 
            perror("ERROR on accept");
        pid = fork();
        if (pid < 0)
            perror("ERROR on fork");
        if (pid == 0)  {

            // Wait for message
            // Check if message is OI or TI
            // Echo
            // Send back the window size

            // Receive target 
            // Receive Obstacle
            // Store it as local variables
            // send it to the necessary processes
            // exit(0);
            //---------------------------------------------------------------------
        }else{
            fd_set reading;
            FD_ZERO(&reading);

            for(int i=0; i<NUM_PROCESSES-1;i++){
                FD_SET(rec_pipes[i][0], &reading);
            }
            for (int i = 0; i < NUM_PROCESSES-1; i++) {
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
                            my_write(server_target[1],&data,server_target[0],sizeof(data));
                            my_write(server_window[1],&data,server_window[0],sizeof(data)); 
                            break;

                        default:
                            break;
                        }
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