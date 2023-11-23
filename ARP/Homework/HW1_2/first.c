#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() 
{ 
    int fd1,fd2; 
   
    char * numfifo = "/tmp/numfifo"; 
    mkfifo(numfifo, 0666); 
    char * ackfifo = "/tmp/ackfifo"; 
    mkfifo(ackfifo, 0666); 
  
    char input_string[80], rec_string[5],ack[2];
    ack[0] = 'y'; 
    ack[1] = 'a';

    while (1) 
    { 
        if (ack[0]=='y'){
            fd1 = open(numfifo, O_WRONLY); 
            // fd2 = open(recfifo, O_WRONLY); 

            printf("Please, write two integer numbers, separated by commas (,), or q to quit\n");
            /* to be sure that the previous is executed immediately */
            fflush(stdout);
            /* read a full input line */
            fgets(input_string, 80 , stdin); 
            write(fd1, input_string, strlen(input_string)+1);
            printf("Not ready now\n"); 
            // printf("Ack is");
            ack[0]='n';
            close(fd1); 
            
            
            
            /* if the first input char is q, exit  */
            if (input_string[0] == 'q') exit(EXIT_SUCCESS) ;
        }else if(ack[0]=='n'){
            fd2 = open(ackfifo, O_RDONLY);
            read(fd2, ack, 1); 
            close(fd2);
        }else{
            printf("ERROR: SOMETHING IS WRONG\n");
            exit(1);
        }
        



  
    } 
    return 0; 
} 
