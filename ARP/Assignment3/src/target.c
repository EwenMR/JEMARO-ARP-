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
char target_msg[MSG_LEN];
int window_x, window_y;

void error(char *msg) {
    perror(msg);
    exit(0);
}


// // Checks if target is reached
// void target_update(float *target_pos) {
//     int j=0;
//     for (int i = 0; i < NUM_TARGETS * 2; i += 2) {
//         if(target_pos[i]==0 &&target_pos[i+1]==0){ // Only check the targets that are not 0,0
//         }else{
//             j=i;
//             break;
//         }
//     }

// }


// make the targets coordinates
void makeTargs(){

    for (int i=0; i< NUM_TARGETS*2; i+=2){
        target_pos[i]   = rand() % window_x;
        target_pos[i+1] = rand() % window_y;

    }
    sprintf(target_msg, "T[%d] ", NUM_TARGETS*2);

    for (int i = 0; i < NUM_TARGETS; ++i) {
        // Append obstacle information to target_msg
        sprintf(target_msg + strlen(target_msg), "%.3f,%.3f", 
        (float)target_pos[2*i], (float)target_pos[2*i+1]);

        // Add a separator if there are more obstacles
        if (i < NUM_TARGETS*2 - 1) {
            sprintf(target_msg + strlen(target_msg), "|");
        }
        sprintf(logMessage, "%f %f",target_pos[2*i],target_pos[2*i+1]);
        writeToLogFile(targetlogpath,logMessage);
    }
    writeToLogFile(targetlogpath,"SEND");
    writeToLogFile(targetlogpath,target_msg);
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


void send_target_to_obstacle(int *target_obstacle){
    // Generate targets (only once) and send it to server
    makeTargs(); 
    writeToLogFile(targetlogpath, "generated targets");
    
    sprintf(logMessage, "%f %f", target_pos[0], target_pos[1]);
    writeToLogFile(targetlogpath,logMessage);

    
    my_write(target_obstacle[1],&target_pos,target_obstacle[0],sizeof(target_pos));
    writeToLogFile(targetlogpath, "sent targets to obstacle");
    // memcpy(data.target_pos, target_pos, sizeof(target_pos));


}

int main(int argc, char* argv[]){
    clearLogFile(targetlogpath);
    srand((unsigned int)time(NULL));
    // srand(time(NULL));

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    
    setupSocketConnection(argv[3], portno);
    //IDENTIFICATION

    int target_obstacle[2];
    sscanf(argv[1], client_args_format,  &target_obstacle[0], &target_obstacle[1]);
    
    char status[MSG_LEN];
    char TI[]="TI";
    write_then_wait_echo(sockfd, TI, strlen(TI) + 1);
    char rec_msg[MAX_MSG_LEN];
    read_then_echo(sockfd, rec_msg);
    // read(sockfd, rec_msg, strlen(rec_msg));
    writeToLogFile(targetlogpath,"WINDOWSIZE");
    writeToLogFile(targetlogpath,rec_msg);

    sscanf(rec_msg,"%d %d", &window_x,&window_y);
    

    send_target_to_obstacle(target_obstacle);
    

    
    
    int ready;
        int bytes_read, bytes_written;

    write_then_wait_echo(sockfd,target_msg,strlen(target_msg));

    while(1) {
        //NON? BLOCK READ
        read_then_echo_unblocked(sockfd, status, window_x, window_y);
        //IF GE, GENERATE
        if(strcmp(status, "GE") == 0){
            send_target_to_obstacle(target_obstacle);
        }
        //IF STOP, STOP
        else if (strcmp(status, "STOP") == 0){
            sprintf(logMessage, "GAME TERMINATED");
            writeToLogFile(targetlogpath, logMessage);
            exit(EXIT_SUCCESS);
        }
        //ELSE
        
        sleep(5);
    }

    // Clean up
    close(sockfd);
    return 0;
}
