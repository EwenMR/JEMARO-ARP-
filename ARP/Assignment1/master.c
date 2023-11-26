//master.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_MSG_LEN 100

void summon(char **programArgs){
    execvp(programArgs[0], programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]){
    char *argsServer[] = {"./build/server","placeholder", NULL};
    char *argsWindow[] = {"konsole",  "-e", "./build/window", "placeholder",NULL};
    char *argsDrone[] = {"konsole",  "-e", "./build/drone", "placeholder",NULL};
    char *argsKeyboard[] = {"konsole",  "-e","./build/keyboard", "placeholder",NULL};
    char *argsSave[] = {"konsole",  "-e", "./build/save", NULL};

    int window_keyboard[2];
    int keyboard_window[2];

    pipe(window_keyboard);
    pipe(keyboard_window);

    // if (pipe(window_keyboard) == -1 || pipe(keyboard_window) == -1) {
    //     perror("pipe");
    //     exit(EXIT_FAILURE);
    // }

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
            

            if(i==0){
                summon(argsServer);
            }else if(i==1){ 
                char args[MAX_MSG_LEN];
                sprintf(args,"%d %d|%d %d",window_keyboard[0],window_keyboard[1],keyboard_window[0],keyboard_window[1]);
                argsWindow[3]=args;
                summon(argsWindow);
            }else if(i==2){
                char args[MAX_MSG_LEN];
                sprintf(args,"%d %d|%d %d",window_keyboard[0],window_keyboard[1],keyboard_window[0],keyboard_window[1]);
                argsKeyboard[3]=args;

                summon(argsKeyboard);
            // }else if(i==3){
                // summon(argsTest);

            }else{
                // summon(argsTest);
            }   
        }else {
            printf("Summoned child with pid %d\n", pid);
        }
}
    }
    
    


