#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    char* myfifo = "./myfifo";
    mkfifo(myfifo,0666);
    int fd;
    char a[80];
    a[0]='A';

    
    while(1){
        fd = open(myfifo, O_WRONLY);
        write(fd,a,1);
        usleep(0.1);
    }
    
    return 0;
    
}