#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include "../include/log.c"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <string.h>
#include "../include/constants.h"
#include "../include/utility.c"


float drone_pos[6], obstacle_pos[NUM_OBSTACLES*2],target_pos[NUM_TARGETS*2];
int sockfd;
int window_x, window_y;

void makeObs() {
    for (int i = 0; i < NUM_OBSTACLES*2; i+=2) {
        // generating obstacle coordinates
        obstacle_pos[i]   = rand() % window_x;  // Random value between 0 and 100
        obstacle_pos[i+1] = rand() % window_y;  // Random value between 0 and 100

        for(int j=0; j<NUM_TARGETS; j+=2){
            while (obstacle_pos[i]   >= target_pos[j] - OBS_THRESH   && obstacle_pos[i]   <= target_pos[j] + OBS_THRESH && 
                   obstacle_pos[i+1] >= target_pos[j+1] - OBS_THRESH && obstacle_pos[i+1] <= target_pos[j+1] + OBS_THRESH) {
                // Regenerate obstacle-coordinate
                obstacle_pos[i]   = rand()%window_x;  // Random value between 0 and 100
                obstacle_pos[i+1] = rand()%window_y;  // Random value between 0 and 100
            }
        }
    }
}

int get_time(){
    int seconds,milliseconds;
    
    time_t t_now;
    time(&t_now);
    struct tm *local = localtime(&t_now);
    struct timeval time_now;
    gettimeofday(&time_now, NULL);
    local = localtime(&time_now.tv_sec);
    seconds = local -> tm_sec;

    return seconds;
}

bool update(int remainder){
    int seconds;
    seconds=get_time();
    
    if(seconds%OBSTACLE_REFRESH_RATE == remainder){ // 
        return true;
    }else{
        return false;
    }
}

void setupSocketConnection(char *hostname, int portno) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
}

// Function definitions for makeObs, get_time, and update remain unchanged

int main(int argc, char* argv[]){

    clearLogFile(obstaclelogpath);
    writeToLogFile(obstaclelogpath, "START");
    sleep(1);
    srand((unsigned int)time(NULL));
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    char hostname[50];
    strcpy(hostname, argv[3]);
    setupSocketConnection(argv[3], portno);

    char OI[]="OI";
    write_then_wait_echo(sockfd, OI, strlen(OI) + 1);

    char rec_msg[MAX_MSG_LEN];
    read_then_echo(sockfd, rec_msg);

    writeToLogFile(obstaclelogpath,"WINDOWSIZE");
    writeToLogFile(obstaclelogpath,rec_msg);
    sscanf(rec_msg,"%d %d", &window_x,&window_y);

    int target_obstacle[2];
    sscanf(argv[1], client_args_format,  &target_obstacle[0], &target_obstacle[1]);
    writeToLogFile(obstaclelogpath, "got pipes");
    my_read(target_obstacle[0],&target_pos,target_obstacle[1],sizeof(target_pos));


    char logmessage[80];
    sprintf(logmessage, "%f %f", target_pos[0], target_pos[1]);
    writeToLogFile(obstaclelogpath,logmessage);


    char status[MSG_LEN];

    struct shared_data data; // Ensure this struct is defined to match the expected data format for both sending and receiving
    int seconds, new_seconds, remainder;
    bool first = true;

    // USE PIPES TO RECEIVE TARGET POSITION
    // STORE IT AS LOCAL VARIABLES

    while(1) {
        if(first) {
            makeObs(); // Generate initial obstacles
            writeToLogFile(obstaclelogpath, "generated obstacles");
            first = false;
            seconds = get_time();
            remainder = seconds % OBSTACLE_REFRESH_RATE;
        } else {
            new_seconds = get_time();
            if(update(remainder) == true && seconds != new_seconds) {
                makeObs(); // Regenerate obstacles
                writeToLogFile(obstaclelogpath, "regenerated targets");
                seconds = new_seconds;
            }
        }

        // Prepare data to be sent
        // Assuming data.obst_pos and other necessary fields are properly declared in shared_data
        memcpy(data.obst_pos, obstacle_pos, sizeof(obstacle_pos));

        char obstacle_msg[MSG_LEN];
        sprintf(obstacle_msg, "O[%d] ", NUM_OBSTACLES*2);

        for (int i = 0; i < NUM_OBSTACLES*2; ++i) {
            // Append obstacle information to obstacle_msg
            sprintf(obstacle_msg + strlen(obstacle_msg), "%.3f,%.3f", 
            (float)obstacle_pos[2*i], (float)obstacle_pos[2*i+1]);

            // Add a separator if there are more obstacles
            if (i < NUM_OBSTACLES*2 - 1) {
                sprintf(obstacle_msg + strlen(obstacle_msg), "|");
            }
        }
        // Write the string to the socket
        // if (write(sockfd, obstacle_msg, strlen(obstacle_msg) + 1) < 0) {
        //     writeToLogFile(obstaclelogpath,"ERROR writing to socket");
        // }else{
        //     writeToLogFile(obstaclelogpath, obstacle_msg);
        // }
        

        int ready;
        int bytes_read, bytes_written;

        bytes_written = write(sockfd, obstacle_msg, strlen(obstacle_msg));
        writeToLogFile(obstaclelogpath,"SEND");
        writeToLogFile(obstaclelogpath, obstacle_msg);
        if (bytes_written < 0) {perror("ERROR writing to socket");}

        // Clear the buffer
        bzero(obstacle_msg, MSG_LEN);

        while (obstacle_msg[0] == '\0'){
            // Data is available for reading, so read from the socket
            bytes_read = read(sockfd, obstacle_msg, bytes_written);
            if (bytes_read < 0) {perror("ERROR reading from socket");} 
            else if (bytes_read == 0) {printf("Connection closed!\n"); return 0;}
        }
        writeToLogFile(obstaclelogpath,"ECHO");
        writeToLogFile(obstaclelogpath,obstacle_msg);

        
        // non blocking read and echo for STOP
        read_then_echo_unblocked(sockfd, status, window_x, window_y);
        if(strcmp(status, "STOP") == 0){
            writeToLogFile(obstaclelogpath, "GAME TERMINATED");
            exit(EXIT_SUCCESS);
        }





        // sleep(2);
        usleep(50000); // Control the frequency of updates
    }

    close(sockfd); // Clean up the socket
}


