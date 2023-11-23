//master.c
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
    char *argsServer[] = {"konsole",  "-e", "./build/server", NULL};
    char *argsWindow[] = {"konsole",  "-e", "./build/window", NULL};
    char *argsDrone[] = {"konsole",  "-e", "./build/drone", NULL};
    char *argsKeyboard[] = {"konsole",  "-e", "./build/keyboard", NULL};
    // char *argsWatchdog[] = {"konsole",  "-e", "./build/watchdog", NULL};


    char *argsTest[] = {"konsole",  "-e", "./build/test", NULL};
    // char *argsLogger[] = {"konsole",  "-e", "./logger", NULL};


    for (int i=0; i<5; i++){
        pid_t pid = fork();
        if(!pid){
            if (pid < 0) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }
            else if(i==0){
                summon(argsServer);
            }else if(i==1){
                summon(argsWindow);
            }else if(i==2){
                summon(argsDrone);
            }else if(i==3){
                summon(argsKeyboard);
            }else{
                // summon(argsTest);
            }   
        }else {
            printf("Summoned child with pid %d\n", pid);
        }
}
    }
    
    


