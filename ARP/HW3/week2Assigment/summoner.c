#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>

void summon(char** programArgs){
    execvp("konsole",programArgs);
    perror("Execution failed");

    //avoid unwanted forking
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    char* argsFirst[] = {"konsole", "-e", "./first", NULL};
    char* argsSecond[] = {"konsole", "-e", "./second", NULL};

    for (int i = 0; i < 2; i++) {
        // Summoning processes as childs of the caller
        pid_t pid = fork();

        if(pid < 0){
            exit(EXIT_FAILURE);
        }else if(!pid){
            if(i == 0){
                summon(argsFirst);
            }else if(i == 1){
                summon(argsSecond);
            }
        }else{
            // Continuing with father
            printf("Summoned child with pid: %d\n", pid);
        }
    }
    return EXIT_SUCCESS;
}
