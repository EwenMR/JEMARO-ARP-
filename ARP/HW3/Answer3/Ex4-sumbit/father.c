#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void summon(char** programArgs){
    execvp("konsole",programArgs);
    perror("Execution failed");

    //avoid unwanted forking
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int childExitStatus;
    char* argsFirst[] = {"konsole", "-e", "./writer", NULL};
    char* argsSecond[] = {"konsole", "-e", "./reader", "0", NULL};

    for (int i = 0; i < 3; i++) {
        // Summoning processes as childs of the caller
        pid_t pid = fork();

        if(pid < 0){
            exit(EXIT_FAILURE);
        }else if(!pid){
            if(i == 0){
                summon(argsFirst);
            }else {
                //this sets the "name" of the reader
                argsSecond[3] = (i==2)? "1" : "2";

                summon(argsSecond);
            }
        }else{
            // Continuing with father
            printf("Summoned child with pid: %d\n", pid);
            fflush(stdout);
        }
    }

    for (int i = 0; i < 3; i++) {
        //waits for childrens to end
        int finishedPid =  wait(&childExitStatus);
        //if the childs have exited normally print their pid and status
        if(WIFEXITED(childExitStatus)){
            printf("Child %d terminated with exit status: %d\n", finishedPid, WEXITSTATUS(childExitStatus));
        }
    }

    printf("Childs exited and FIFO closed, father closing\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}
