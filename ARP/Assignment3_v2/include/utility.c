// #ifndef UTILITY_H
// #define UTILITY_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "./constants.h"

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
int read_then_echo_unblocked(int sockfd, char socket_msg[]) {
    int ready;
    int bytes_read, bytes_written;
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

    // Print the received message
    printf("[SOCKET] Received: %s\n", socket_msg);

    // Echo the message back to the client
    bytes_written = write(sockfd, socket_msg, bytes_read);
    if (bytes_written < 0) {perror("ERROR writing to socket");}
    else{printf("[SOCKET] Echo sent: %s\n", socket_msg); return 1;}
}

// Writes a message into the socket, then loops/waits until a valid echo is read.
void write_then_wait_echo(int sockfd, char socket_msg[], size_t msg_size){
    int ready;
    int bytes_read, bytes_written;

    bytes_written = write(sockfd, socket_msg, msg_size);
    if (bytes_written < 0) {perror("ERROR writing to socket");}
    printf("[SOCKET] Sent: %s\n", socket_msg);

    // Clear the buffer
    bzero(socket_msg, MSG_LEN);

    while (socket_msg[0] == '\0'){
        // Data is available for reading, so read from the socket
        bytes_read = read(sockfd, socket_msg, bytes_written);
        if (bytes_read < 0) {perror("ERROR reading from socket");} 
        else if (bytes_read == 0) {printf("Connection closed!\n"); return;}
    }
    // Print the received message
    printf("[SOCKET] Echo received: %s\n", socket_msg);
}

// Reads a message from the pipe with select() system call.
int read_pipe_unblocked(int pipefd, char msg[]){
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    fd_set read_pipe;
    FD_ZERO(&read_pipe);
    FD_SET(pipefd, &read_pipe);

    char buffer[MSG_LEN];

    int ready_km = select(pipefd + 1, &read_pipe, NULL, NULL, &timeout);
    if (ready_km == -1) {perror("Error in select");}

    if (ready_km > 0 && FD_ISSET(pipefd, &read_pipe)) {
        ssize_t bytes_read_pipe = read(pipefd, buffer, MSG_LEN);
        if (bytes_read_pipe > 0) {
            strcpy(msg, buffer);
            return 1;
        }
        else{return 0;}
    }
}

// Reads a message from the socket, then does an echo.
void read_then_echo(int sockfd, char socket_msg[]){
    int bytes_read, bytes_written;
    bzero(socket_msg, MSG_LEN);

    // READ from the socket
    bytes_read = read(sockfd, socket_msg, MSG_LEN - 1);
    if (bytes_read < 0) perror("ERROR reading from socket");
    else if (bytes_read == 0) {return;}  // Connection closed
    else if (socket_msg[0] == '\0') {return;} // Empty string
    printf("[SOCKET] Received: %s\n", socket_msg);
    
    // ECHO data into socket
    bytes_written = write(sockfd, socket_msg, bytes_read);
    if (bytes_written < 0) {perror("ERROR writing to socket");}
    printf("[SOCKET] Echo sent: %s\n", socket_msg);
}

void error(char *msg);
void setupSocketConnection(char *hostname, int portno);

// #endif