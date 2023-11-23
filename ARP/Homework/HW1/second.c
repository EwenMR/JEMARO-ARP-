
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() 
{ 
    int num_fd,ack_fd; 
    char * numfifo = "/tmp/numfifo"; 
    mkfifo(numfifo, 0666); 
    char * ackfifo = "/tmp/ackfifo"; 
    mkfifo(ackfifo, 0666); 
    char str1[80], str2[80]; 
    char format_string[80]="%d,%d";
    int n1, n2;
    double mean;
    char ack[2] = "k";

    while (1) 
    { 
        num_fd = open(numfifo,O_RDONLY); 
        read(num_fd, str1, 80); 
 
        /* if the first input char is q, exit  */
        if (str1[0] == 'q') exit(EXIT_SUCCESS) ;
        /* read numbers from input line */
        sscanf(str1, format_string, &n1, &n2);
        mean = (n1 + n2) / 2.0; 
        printf("mean value is: %f, sum is: %d\n", mean, n1 + n2); 
        ack_fd = open(ackfifo,O_WRONLY);
        write(ack_fd,ack,strlen(ack)+1);
        printf("k");
        close(ack_fd);
        close(num_fd); 
    } 
    return 0; 
} 
