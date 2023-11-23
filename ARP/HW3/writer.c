#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>

#define LOGINFO "\x1b.[m32 [LOG INFO] \x1b.[m1 "


int main (int argc, char* argv[]){
    char target[80], input[80];
    char* myfifo = "./myfifo";
    mkfifo(myfifo, 0666);
    int targetfd, sendfd;


    while(1){
        printf("Enter 1 or 2 to select which konsole you want to send the message to, or q to quit\n");
        fflush(stdout);

        fgets(target,80,stdin);

        if (target[0] == 'q'){
            targetfd = open(myfifo, O_WRONLY);
            write(targetfd, target, 80);
            close(targetfd);
            unlink(myfifo);
            exit(EXIT_SUCCESS);

        }else if(target[0] == '1' || target[0] == '2'){
            fflush(stdout);
            printf("Enter two numbers separated by commas ex) 3,5\n or q to quit\n");
            fgets(input, 80, stdin);

            if (input[0] =='q'){
                targetfd = open(myfifo, O_WRONLY);
                write(targetfd, target, 80);
                close(targetfd);
                unlink(myfifo);
                exit(EXIT_SUCCESS);

            }else{
                target[strcspn(target, "\r\n")] = 0;
                strcat(target, "|");
                strcat(target, input);
                target[strcspn(target, "\r\n")] = 0;

                sendfd = open(myfifo, O_WRONLY);
                write(sendfd,target,80);
                close(sendfd);
                unlink(myfifo);
            }
        }else{
            printf("Wrong input");
        }


    }
    return 0;
}