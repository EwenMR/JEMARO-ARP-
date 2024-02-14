#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/utility.c"
#include "../include/constants.h"
#include <time.h>
#include <math.h>
#include "../include/log.c"




// Global variables
double target_pos[NUM_TARGETS*2],obstacle_pos[NUM_OBSTACLES*2],drone_pos[6];
struct shared_data data;
int sockfd;


void error(char *msg) {
    perror(msg);
    exit(0);
}


// Checks if target is reached
void target_update(double *drone_pos, double *target_pos) {
    int j=0;
    for (int i = 0; i < NUM_TARGETS * 2; i += 2) {
        if(target_pos[i]==0 &&target_pos[i+1]==0){ // Only check the targets that are not 0,0
        }else{
            j=i;
            break;
        }
    }

}


// make the targets coordinates
void makeTargs(double drone_pos[]){
    for (int i=0; i< NUM_TARGETS*2; i+=2){
        target_pos[i]   = ((double)rand() / RAND_MAX) * BOARD_SIZE;
        target_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;

        // check if they aren't within threshold of drone
        while(fabs(target_pos[i] - drone_pos[4]) < TARGET_THRESH  && fabs(target_pos[i+1] - drone_pos[5]) < TARGET_THRESH) {
            // Regenerate target_pos coordinates
            target_pos[i] = ((double)rand() / RAND_MAX) * BOARD_SIZE;
            target_pos[i+1] = ((double)rand() / RAND_MAX) * BOARD_SIZE;
        }
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
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
}


int main(int argc, char* argv[]){
    clearLogFile(targetlogpath);

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    
    setupSocketConnection(argv[3], portno);

    

    // Generate targets (only once) and send it to server
    makeTargs(drone_pos); 
    memcpy(data.target_pos, target_pos, sizeof(target_pos));

    // Writing the initial data to the server
    if (write(sockfd, &target_pos, sizeof(target_pos)) < 0) 
        error("ERROR writing to socket");
    writeToLogFile(targetlogpath, "TARGET: Initial target_pos sent to server");

    while(1) {
        
        // char test[50];
        // strcpy(test, "TARGET");
        // if (write(sockfd, test, sizeof(test)) < 0) 
        //     error("ERROR writing to socket");
        // sleep(10000);

        // strcpy(test, "TARGET");
        // if (write(sockfd, test, sizeof(test)) < 0) 
        //     error("ERROR writing to socket");

        // Receive updated drone position from server
        // if (read(sockfd, &data, sizeof(data)) < 0) 
        //     error("ERROR reading from socket");
        // memcpy(drone_pos, data.drone_pos, sizeof(data.drone_pos));
        // writeToLogFile(targetlogpath, "TARGET: drone_pos received from server");
        
        // target_update(drone_pos, target_pos); // Check if drone reached the target

        // Copy updated target position to shared data and send it
        // memcpy(data.target_pos, target_pos, sizeof(target_pos));
        // if (write(sockfd, &data, sizeof(data)) < 0) 
        //     error("ERROR writing to socket");
        // writeToLogFile(targetlogpath, "TARGET: Updated target_pos sent to server");
    }

    // Clean up
    close(sockfd);
}
