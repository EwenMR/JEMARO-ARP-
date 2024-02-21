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


double drone_pos[6], obstacle_pos[NUM_OBSTACLES*2],target_pos[NUM_TARGETS*2];
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
    clearLogFile(obstaclelogpath);
    writeToLogFile(obstaclelogpath, "START");
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    int portno = atoi(argv[2]);
    char hostname[50];
    strcpy(hostname, argv[3]);
    setupSocketConnection(argv[3], portno);

    int target_obstacle[2];
    sscanf(argv[1], client_args_format,  &target_obstacle[0], &target_obstacle[1]);
    writeToLogFile(obstaclelogpath, "got pipes");
    my_read(target_obstacle[0],&target_pos,target_obstacle[1],sizeof(target_pos));

    char logmessage[80];
    sprintf(logmessage, "%f %f", target_pos[0], target_pos[1]);
    writeToLogFile(obstaclelogpath,logmessage);

    char OI[2];
    OI[0] = 'O';
    OI[1] = 'I';
    if (write(sockfd, OI, strlen(OI) + 1) < 0) {
        error("ERROR writing to socket");
    }


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

        


        //-------------------------------------------------------------------------
        int totalDigits = 0;
        for (int i = 0; i < NUM_OBSTACLES * 2; i++) {
            // Count the number of digits for each float
            int numDigits = snprintf(NULL, 0, "%.2f", obstacle_pos[i]);
            totalDigits += numDigits;
        }
        totalDigits += (NUM_OBSTACLES * 2 - 1) * 2; // Account for spaces between numbers and the null terminator
        totalDigits += 2; // Account for the leading 'T' and the null terminator

        // Create a string buffer
        char str[totalDigits];

        // Add 'T' as the first character in the string
        str[0] = 'O';

        // Convert the array elements to strings and concatenate them with spaces
        int offset = 1;
        for (int i = 0; i < NUM_OBSTACLES * 2; i++) {
            offset += snprintf(str + offset, totalDigits - offset, "%.2f ", obstacle_pos[i]);
        }

        // Null terminate the string
        str[offset - 1] = '\0'; // Remove the extra space at the end
        writeToLogFile(obstaclelogpath, str);

        // Write the string to the socket
        if (write(sockfd, str, strlen(str) + 1) < 0) {
            error("ERROR writing to socket");
        }
        writeToLogFile(obstaclelogpath, "sent obstacles to server");
        //---------------------------------------------------------------------





        sleep(3);
        usleep(50000); // Control the frequency of updates
    }

    close(sockfd); // Clean up the socket
}


