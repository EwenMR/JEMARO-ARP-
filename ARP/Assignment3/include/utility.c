// #ifndef UTILITY_H
// #define UTILITY_H

#include <unistd.h>
#include "./constants.h"
#include <time.h>
#include <stdio.h>      // For standard I/O operations
#include <stdlib.h>     // For standard library functions such as exit()
#include <unistd.h>     // For POSIX operating system API
#include <string.h>     // For string manipulation functions
#include <sys/types.h>  // For data types used in system calls
#include <sys/socket.h> // For socket-related functions
#include <netinet/in.h> // For Internet address structures
#include <netdb.h>      // For network database operations such as gethostbyname()

void writeToLogFile(const char *logpath, const char *logMessage) {
    FILE *logFile = fopen(logpath, "a");  // Open file in append mode

    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    // Get the current time
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);

    // Write log entry with timestamp
    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday,
            timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec, logMessage);

    fclose(logFile);
}



void clearLogFile(const char *logpath) {
    FILE *logFile = fopen(logpath, "w");  // Open file in write mode (truncates if exists)

    if (logFile == NULL) {
        perror("Error opening log file");
        // Handle the error as needed
    } else {
        fclose(logFile);
        printf("Logfile cleared successfully.\n");
    }
}

void my_write(int fd,void *data,int fd2,int size){
    int ret=write(fd,data,size);
    if(ret==-1){
        perror("writing error\n");
        close(fd);
        close(fd2);
        exit(EXIT_FAILURE);
    }
}

void my_read(int fd,void *data,int fd2, int size){
    int ret=read(fd,data,size);
    if(ret==-1){
        perror("reading error\n");
        close(fd);
        close(fd2);
        exit(EXIT_FAILURE);
    }
}


// Reads a message from the socket, with select() system call, then does an echo.
int read_then_echo_unblocked(int sockfd, char socket_msg[],float windowx, float windowy) {
    int ready;
    int bytes_read, bytes_written,bytes_written2;
    char window_msg[MAX_MSG_LEN];
    fd_set read_fds;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    // Clear the buffer
    bzero(socket_msg, MSG_LEN);

    // Initialize the set of file descriptors to monitor for reading
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);

    // Use select to check if the socket is ready for reading
    ready = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
    if (ready < 0) {perror("ERROR in select");} 
    else if (ready == 0) {return 0;}  // No data available

    // Data is available for reading, so read from the socket
    bytes_read = read(sockfd, socket_msg, MSG_LEN - 1);
    if (bytes_read < 0) {perror("ERROR reading from socket");} 
    else if (bytes_read == 0) {return 0;}  // Connection closed
    else if (socket_msg[0] == '\0') {return 0;} // Empty string

    // Echo the message back to the client
    bytes_written = write(sockfd, socket_msg, bytes_read);

    if (bytes_written < 0){perror("ERROR writing to socket");}
    else{return 1;}
}

// Writes a message into the socket, then loops/waits until a valid echo is read.
void write_then_wait_echo(int sockfd, char socket_msg[], size_t msg_size){
    int ready;
    int bytes_read, bytes_written,bytes_read2;
    char window_msg[MAX_MSG_LEN];

    bytes_written = write(sockfd, socket_msg, msg_size);
    if (bytes_written < 0) {perror("ERROR writing to socket");}

    // Clear the buffer
    bzero(socket_msg, MSG_LEN);

    while (socket_msg[0] == '\0'){
        // Data is available for reading, so read from the socket
        bytes_read = read(sockfd, socket_msg, bytes_written);
        if (bytes_read < 0) {perror("ERROR reading from socket");} 
        else if (bytes_read == 0) {printf("Connection closed!\n"); return;}
        printf("GOT echoed");
    }
    
}

// Reads a message from the socket, then does an echo.
void read_then_echo(int sockfd, char* socket_msg){
    int bytes_read, bytes_written;
    bzero(socket_msg, sizeof(socket_msg));

    // READ from the socket
    bytes_read = read(sockfd, socket_msg, sizeof(socket_msg));
    if (bytes_read < 0) perror("ERROR reading from socket");
    else if (bytes_read == 0) {return;}  // Connection closed
    else if (socket_msg[0] == '\0') {return;} // Empty string
    
    // ECHO data into socket
    bytes_written = write(sockfd, socket_msg, bytes_read);
    if (bytes_written < 0) {perror("ERROR writing to socket");}
}

void just_read(int sockfd, char socket_msg[]){
    int bytes_read, bytes_written;
    bzero(socket_msg, MSG_LEN);

    // READ from the socket
    bytes_read = read(sockfd, socket_msg, MSG_LEN - 1);
    if (bytes_read < 0) perror("ERROR reading from socket");
    else if (bytes_read == 0) {return;}  // Connection closed
    else if (socket_msg[0] == '\0') {return;} // Empty string
}

void error(char *msg);
void setupSocketConnection(char *hostname, int portno);

// #endif
