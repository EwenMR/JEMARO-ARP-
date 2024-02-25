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
float target_pos[NUM_TARGETS*2],obstacle_pos[NUM_OBSTACLES*2];
struct shared_data data;
int sockfd;
char logmessage[MSG_LEN];
char target_msg[MSG_LEN];

void error(char *msg) {
    perror(msg);
    exit(0);
}


// Checks if target is reached
void target_update(float *target_pos) {
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
void makeTargs(){
    for (int i=0; i< NUM_TARGETS*2; i+=2){
        target_pos[i]   = rand() % BOARD_SIZE;
        target_pos[i+1] = rand() % BOARD_SIZE;

    }
}

// char *change_targ_str(){
//     char targ_str[NUM_TARGETS];
//     int offset = sprintf(targ_str, "T[%d]", NUM_TARGETS);
//     for (int i = 0; i < NUM_TARGETS; ++i) {
//         offset += sprintf(targ_str + offset, "%.3f,%.3f", (float)target_pos[2*i], (float)target_pos[2*i+1]);
//     }
//     return *targ_str;
// }


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


void send_target_to_obstacle(int *target_obstacle){
    // Generate targets (only once) and send it to server
    makeTargs(); 
    writeToLogFile(targetlogpath, "generated targets");
    
    sprintf(logmessage, "%f %f", target_pos[0], target_pos[1]);
    writeToLogFile(targetlogpath,logmessage);

    
    my_write(target_obstacle[1],&target_pos,target_obstacle[0],sizeof(target_pos));
    writeToLogFile(targetlogpath, "sent targets to obstacle");
    // memcpy(data.target_pos, target_pos, sizeof(target_pos));


}

int main(int argc, char* argv[]){
    clearLogFile(targetlogpath);

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    
    setupSocketConnection(argv[3], portno);
    //IDENTIFICATION
    char TI[]="TI";
    if (write(sockfd, TI, strlen(TI) + 1) < 0) {
        error("ERROR writing to socket");
    }

    int target_obstacle[2];
    sscanf(argv[1], client_args_format,  &target_obstacle[0], &target_obstacle[1]);
    send_target_to_obstacle(target_obstacle);


    
    
    
    

    while(1) {
        sprintf(target_msg, "T[%d] ", NUM_TARGETS*2);

        for (int i = 0; i < NUM_TARGETS*2; ++i) {
            // Append obstacle information to target_msg
            sprintf(target_msg + strlen(target_msg), "%.3f,%.3f", 
            (float)target_pos[2*i], (float)target_pos[2*i+1]);

            // Add a separator if there are more obstacles
            if (i < NUM_TARGETS*2 - 1) {
                sprintf(target_msg + strlen(target_msg), "|");
            }
        }
        // Write the string to the socket
        
        // sprintf(logmessage,"%c, %c", target_msg[0],target_msg[1]);
        // writeToLogFile(targetlogpath,logmessage);
        // if (write(sockfd, target_msg, strlen(target_msg) + 1) < 0) {
        //     error("ERROR writing to socket");
        // }
        // writeToLogFile(targetlogpath, target_msg);
        // sleep(5);



        int ready;
        int bytes_read, bytes_written;
        writeToLogFile(targetlogpath,"SEND");
        writeToLogFile(targetlogpath,target_msg);
        bytes_written = write(sockfd, target_msg, strlen(target_msg));
        if (bytes_written < 0) {perror("ERROR writing to socket");}
        printf("[SOCKET] Sent: %s\n", target_msg);

        // Clear the buffer
        bzero(target_msg, MSG_LEN);

        while (target_msg[0] == '\0'){
            // Data is available for reading, so read from the socket
            bytes_read = read(sockfd, target_msg, bytes_written);
            if (bytes_read < 0) {perror("ERROR reading from socket");} 
            else if (bytes_read == 0) {printf("Connection closed!\n"); return 0;}
        }
        // Print the received message
        // sprintf(logmessage,"[SOCKET] Echo received: %s\n", target_msg);
        writeToLogFile(targetlogpath, "ECHO");
        writeToLogFile(targetlogpath,target_msg);

    }

    // Clean up
    close(sockfd);
    return 0;
}