#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() {   
    int num_fd,ack_fd; 
    
   
    char * numfifo = "/tmp/numfifo"; 
    mkfifo(numfifo, 0666); 
    char * ackfifo = "/tmp/ackfifo"; 
    mkfifo(ackfifo, 0666); 
    int ready=1;
    char input_string[80], rcv_string[80]; 
    char ack[2] = "p";


    while (1) 
    { 
        if (ready==1){
            printf("ready");
            num_fd = open(numfifo, O_WRONLY); 

            printf("Please, write two integer numbers, separated by commas (,), or q to quit\n");
            /* to be sure that the previous is executed immediately */
            fflush(stdout);
            /* read a full input line */
            fgets(input_string, 80 , stdin); 
            sleep(4);
            write(num_fd, input_string, strlen(input_string)+1);

            ready=0;
            close(num_fd); 
            /* if the first input char is q, exit  */
            if (input_string[0] == 'q') exit(EXIT_SUCCESS);
        }
        else{
            printf("not ready");
            ack_fd = open(ackfifo, O_RDWR); 
            read(ack_fd,rcv_string,1);
            if (rcv_string[0] == 'k'){
                ready=1;
                write(ack_fd, ack, strlen(ack)+1);
                printf("p");
            }
            close(ack_fd);
            
            
        }
        
  
    } 
    return 0; 
} 
