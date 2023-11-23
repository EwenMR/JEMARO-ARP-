#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void summon(char** konsole){
    execvp("konsole", konsole);
    perror("Execution failed\n");

    exit(EXIT_FAILURE);
}



int main(int argc, char** argv[]){
    int childExitStatus;
    char *argsWriter[] = {"konsole", "-e", "./writer", NULL};
    char *argsReader[] = {"konsole", "-e", "./reader", "0", NULL};

    for(int i=0;i<3;i++){
        pid_t pid = fork();

        if (!pid){
            if (i==0){
            summon(argsWriter);
            }else{
                argsReader[3] = i==1? "1":"2";
                summon(argsReader);
            }
        }else{
            printf("child with pid of %d is summoned\n" ,pid);
            fflush(stdout);
        }
        
    }

    for(int i=0;i<3;i++){
        int finishedPID = wait(&childExitStatus);
        if(WIFEXITED(childExitStatus)){
            printf("Child %d exited, so closing down everything\n", finishedPID);
            fflush(stdout);
            
        }
    }
    return EXIT_SUCCESS;

}