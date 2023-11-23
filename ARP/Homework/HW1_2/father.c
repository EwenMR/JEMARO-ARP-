#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


void summon(char** program){
    execvp(program[0], program);
    perror("Summon failed\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    int childState;
    char* argsfirst[] = {"konsole", "-e", "./first", NULL};
    char* argssecond[] = {"konsole", "-e", "./second", "0", NULL};

    for(int i=0; i<3; i++){
        pid_t pid = fork();

        if (pid < 0){
            exit(EXIT_FAILURE);
        }else if (!pid){
            if (i==0){
                summon(argsfirst);
            }else{
                argssecond[3] = (i==1) ? "1" : "2";
                summon(argssecond);
            }
        }else{
            printf("Executed a child with a pid of %d", pid);
            fflush(stdout);
        }
    }

    for(int i=0; i<3; i++){
        int finishedPID = wait(&childState);
        if(WIFEXITED(childState)){
            printf("child %d has terminated\n", finishedPID);
        }else{
            perror("Child is not terminated correctly\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("Father terminating because the child has terminated\n");
    fflush(stdout);

}