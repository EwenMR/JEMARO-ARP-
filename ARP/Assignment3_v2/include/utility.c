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



void error(char *msg);
void setupSocketConnection(char *hostname, int portno);

// #endif