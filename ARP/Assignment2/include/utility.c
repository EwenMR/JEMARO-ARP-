// #ifndef UTILITY_H
// #define UTILITY_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void my_write(int fd,int* input,int fd2,int size){
    int ret=write(fd,input,size);
    // if(ret==-1){
    //     perror("writing error\n");
    //     close(fd);
    //     close(fd2);
    //     exit(EXIT_FAILURE);
    // }
}

void my_read(int fd,int* input,int fd2, int size){
    int ret=read(fd,input,size);
    // if(ret==-1){
    //     perror("reading error\n");
    //     close(fd);
    //     close(fd2);
    //     exit(EXIT_FAILURE);
    // }
}


// #endif