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

// Functions
void error(char *);
void makeTargs();
void setupSocketConnection(char *, int );
void send_target_to_obstacle(int *);



int main(int argc, char* argv[]){
    clearLogFile(targetlogpath);
    srand((unsigned int)time(NULL));

    ////////////////////////////////////////////////
    /*LOCAL VARIABLES*/
    ////////////////////////////////////////////////
    int target_obstacle[2];
    char status[5];
    char TI[]="TI";
    char rec_msg[MSG_LEN];
    int bytes_read, bytes_written;



    ////////////////////////////////////////////////
    /*MESSAGES*/
    ////////////////////////////////////////////////

    //IDENTIFICATION
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    setupSocketConnection(argv[3], portno);
    write_then_wait_echo(sockfd, TI, strlen(TI) + 1);
    
    // Get window size
    read_then_echo(sockfd, rec_msg);
    writeToLogFile(targetlogpath,"WINDOWSIZE");
    writeToLogFile(targetlogpath,rec_msg);
    sscanf(rec_msg,"%d %d", &window_x,&window_y);
    
    // Generate and send target position to obstacle
    sscanf(argv[1], client_args_format,  &target_obstacle[0], &target_obstacle[1]);
    send_target_to_obstacle(target_obstacle);

    // Send target positions to server
    write_then_wait_echo(sockfd,target_msg,strlen(target_msg));


    ///////////////////////////////////////////////
    /*CHECK FOR GE AND STOP*/
    ///////////////////////////////////////////////
    while(1) {
        just_read(sockfd, rec_msg);
        writeToLogFile(targetlogpath,rec_msg);
        if(strcmp(rec_msg, "GE") == 0){
            writeToLogFile(targetlogpath,rec_msg);
            makeTargs();
            write_then_wait_echo(sockfd,target_msg,strlen(target_msg));

        }else if (strcmp(rec_msg, "STOP") == 0){
            writeToLogFile(targetlogpath, "GAME TERMINATED");
            exit(EXIT_SUCCESS);
        }else{
            usleep(50000);
        }
    }

    // Clean up
    close(sockfd);
    return 0;
}























///////////////////////////////////////////////////
/*FUNCTION*/
///////////////////////////////////////////////////

void error(char *msg) {
    perror(msg);
    exit(0);
}


// make the targets coordinates
void makeTargs(){
    // Get random positions
    for (int i=0; i< NUM_TARGETS*2; i+=2){
        target_pos[i]   = rand() % window_x;
        target_pos[i+1] = rand() % window_y;
    }

    // Convert it into string
    sprintf(target_msg, "T[%d] ", NUM_TARGETS*2);
    for (int i = 0; i < NUM_TARGETS; ++i) {
        sprintf(target_msg + strlen(target_msg), "%.3f,%.3f", 
        (float)target_pos[2*i], (float)target_pos[2*i+1]);

        // Add a separator if there are more obstacles
        if (i < NUM_TARGETS*2 - 1) {
            sprintf(target_msg + strlen(target_msg), "|");
        }
    }
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

// Generate targets and send it to obstacle
void send_target_to_obstacle(int *target_obstacle){
    makeTargs(); 
    writeToLogFile(targetlogpath, "generated targets");
    
    my_write(target_obstacle[1],&target_pos,target_obstacle[0],sizeof(target_pos));
    writeToLogFile(targetlogpath, "sent targets to obstacle");
}