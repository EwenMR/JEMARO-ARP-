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


double drone_pos[6], obstacle_pos[NUM_OBSTACLES*2], target_pos[NUM_TARGETS*2];
int sockfd;

void makeObs() {
    for (int i = 0; i < NUM_OBSTACLES*2; i+=2) {
        // generating obstacle coordinates
        obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
        obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100

        for(int j=0; j<NUM_TARGETS*2; j+=2){
            while (obstacle_pos[i]   >= target_pos[j] - OBS_THRESH   && obstacle_pos[i]   <= target_pos[j] + OBS_THRESH && 
                   obstacle_pos[i+1] >= target_pos[j+1] - OBS_THRESH && obstacle_pos[i+1] <= target_pos[j+1] + OBS_THRESH) {
                // Regenerate obstacle-coordinate
                obstacle_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
                obstacle_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;  // Random value between 0 and 100
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
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    char hostname[50];
    strcpy(hostname, argv[3]);
    // int portno = PORTNO;
    // setupSocketConnection(argv[2], portno);
    setupSocketConnection(argv[3], portno);

    struct shared_data data; // Ensure this struct is defined to match the expected data format for both sending and receiving
    int seconds, new_seconds, remainder;
    bool first = true;

    while(1) {
        if(first) {
            makeObs(); // Generate initial obstacles
            first = false;
            seconds = get_time();
            remainder = seconds % OBSTACLE_REFRESH_RATE;
        } else {
            new_seconds = get_time();
            if(update(remainder) == true && seconds != new_seconds) {
                makeObs(); // Regenerate obstacles
                seconds = new_seconds;
            }
        }

        // Prepare data to be sent
        // Assuming data.obst_pos and other necessary fields are properly declared in shared_data
        memcpy(data.obst_pos, obstacle_pos, sizeof(obstacle_pos));

        // Send data to server
        // if (write(sockfd, &data, sizeof(data)) < 0) 
        char test[50];
        strcpy(test, "OBSTACLE");
        if (write(sockfd, test, sizeof(test)) < 0) 
            error("ERROR writing to socket");

        // Optionally read a response or acknowledgement from the server
        // if (read(sockfd, &data, sizeof(data)) < 0) 
        //     error("ERROR reading from socket");
        sleep(10);
        usleep(50000); // Control the frequency of updates
    }

    close(sockfd); // Clean up the socket
}


