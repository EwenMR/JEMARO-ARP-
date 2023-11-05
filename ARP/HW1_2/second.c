
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
    
  
    char str1[80], str2[80],ack[2]; 
    char format_string[80]="%d,%d";
    int n1, n2;
    double mean;
    ack[0]='y';

    while (1) 
    { 
        fd1 = open(numfifo,O_RDONLY); 
        read(fd1, str1, 80); 
        printf("%c,%c,%c\n",str1[0],str1[1],str1[2]);
        /* if the first input char is q, exit  */
        if (str1[0] == 'q') exit(EXIT_SUCCESS) ;
        /* read numbers from input line */
        sscanf(str1, format_string, &n1, &n2);
        mean = (n1 + n2) / 2.0; 
        printf("mean value is: %f, sum is: %d\n", mean, n1 + n2); 
        fd2 = open(ackfifo,O_WRONLY);
        write(fd2, ack, strlen(ack)+1); 
        close(fd1); 
        close(fd2);
    } 
    return 0; 
} 
