// #ifndef UTILITY_H
// #define UTILITY_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void my_write(int fd,void* input,int fd2){
    int ret=write(fd,input,sizeof(input));
    if(ret==-1){
        perror("writing error\n");
        close(fd);
        close(fd2);
        exit(EXIT_FAILURE);
    }
}

void my_read(int fd,void* input,int fd2){
    int ret=read(fd,input,sizeof(input));
    if(ret==-1){
        perror("reading error\n");
        close(fd);
        close(fd2);
        exit(EXIT_FAILURE);
    }
}


// #endif