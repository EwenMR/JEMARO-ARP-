#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
  
int main() 
{ 
    int fd; 
   

    while (1) 
    { 
      

	usleep(567000);
	fflush(stdout);
  	printf("%c",'|');
    } 
    return 0; 
} 
