#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>

void summon(char **programArgs){
    execvp("konsole", programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    char *argsServer[] = {"konsole",  "-e", "./server", NULL};
    // char *argsLogger[] = {"konsole",  "-e", "./logger", NULL};


    for (int i=0; i<2; i++){
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if(i==0){
            summon(argsServer);
        }else{
            summon(argsServer);
        }   
}
    }
    
    


