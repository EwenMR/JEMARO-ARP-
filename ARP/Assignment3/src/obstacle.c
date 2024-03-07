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
float window_x, window_y;


void makeObs();
int get_time();
bool update(int);
void setupSocketConnection(char *, int );



int main(int argc, char* argv[]){
    clearLogFile(obstaclelogpath);
    sleep(1); //Do not delete this

    //seed the random number generator
    srand((unsigned int)time(NULL));

    //////////////////////////////////////////////////
    /*SOCKET INITIALIZATION*/
    //////////////////////////////////////////////////
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    char hostname[50];
    strcpy(hostname, argv[3]);
    setupSocketConnection(argv[3], portno);

    
    ////////////////////////////////////////////////
    /*LOCAL VARIABLES*/
    ////////////////////////////////////////////////
    char OI[]="OI";
    char rec_msg[MAX_MSG_LEN];
    int target_obstacle[2];
    int seconds, new_seconds, remainder;
    bool first = true;
    char obstacle_msg[MSG_LEN];
    int bytes_read, bytes_written;


    //////////////////////////////////////////////////
    /*MESSAGES*/
    //////////////////////////////////////////////////

    //Identification
    write_then_wait_echo(sockfd, OI, strlen(OI) + 1);

    //Get Window size from server
    read_then_echo(sockfd, rec_msg);
    writeToLogFile(obstaclelogpath,"WINDOWSIZE");
    writeToLogFile(obstaclelogpath,rec_msg);
    sscanf(rec_msg,"%.3f %.3f", &window_y,&window_x);

    
    //Get target positions (to make sure the obstacles dont go on top of targets)
    sscanf(argv[1], client_args_format,  &target_obstacle[0], &target_obstacle[1]);
    writeToLogFile(obstaclelogpath, "got pipes");
    my_read(target_obstacle[0],&target_pos,target_obstacle[1],sizeof(target_pos));
    sprintf(logMessage, "%f %f", target_pos[0], target_pos[1]);
    writeToLogFile(obstaclelogpath,logMessage);




    while(1) {
        ////////////////////////////////////////
        /*GENERATE OBSTACLES*/
        ////////////////////////////////////////
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



        ///////////////////////////////////////////////
        /*SEND IT TO SERVER*/
        //////////////////////////////////////////////

        //Convert it into a string
        sprintf(obstacle_msg, "O[%d] ", NUM_OBSTACLES*2);
        for (int i = 0; i < NUM_OBSTACLES*2; ++i) {
            sprintf(obstacle_msg + strlen(obstacle_msg), "%.3f,%.3f", 
            (float)obstacle_pos[2*i], (float)obstacle_pos[2*i+1]);

            // Add a separator
            if (i < NUM_OBSTACLES*2 - 1) {
                sprintf(obstacle_msg + strlen(obstacle_msg), "|");
            }
        }
        //send it!
        bytes_written = write(sockfd, obstacle_msg, strlen(obstacle_msg));
        writeToLogFile(obstaclelogpath,"SEND");
        writeToLogFile(obstaclelogpath, obstacle_msg);
        if (bytes_written < 0) {perror("ERROR writing to socket");}
        bzero(obstacle_msg, MSG_LEN); // Clear the buffer


        ////////////////////////////////////////////////
        /*CHECK FOR STOP*/
        ////////////////////////////////////////////////
        while (obstacle_msg[0] == '\0'){
            // Data is available for reading, so read from the socket
            read(sockfd, obstacle_msg,MSG_LEN);
            if(strcmp(obstacle_msg, "STOP") == 0){
            writeToLogFile(obstaclelogpath, "GAME TERMINATED");
            exit(EXIT_SUCCESS);
        }
        }

    
        usleep(50000); // Control the frequency of updates
    }

    close(sockfd); // Clean up the socket
}
















//////////////////////////////////////////////////////////
/*FUNCTIONS*/
//////////////////////////////////////////////////////////

void makeObs() {
    for (int i = 0; i < NUM_OBSTACLES*2; i+=2) {
        // generating obstacle coordinates
        obstacle_pos[i]   = rand() % (int)window_x;  // Random value between 0 and 100
        obstacle_pos[i+1] = rand() % (int)window_y;  // Random value between 0 and 100

        for(int j=0; j<NUM_TARGETS; j+=2){
            while (obstacle_pos[i]   >= target_pos[j] - OBS_THRESH   && obstacle_pos[i]   <= target_pos[j] + OBS_THRESH && 
                   obstacle_pos[i+1] >= target_pos[j+1] - OBS_THRESH && obstacle_pos[i+1] <= target_pos[j+1] + OBS_THRESH) {
                // Regenerate obstacle-coordinate
                obstacle_pos[i]   = rand()%(int)window_x;  // Random value between 0 and 100
                obstacle_pos[i+1] = rand()%(int)window_y;  // Random value between 0 and 100
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

// Check if its time to update the obstacle position
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