// keyboardManager.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "../include/utility.c"


#include "../include/constants.h"

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
    }
}

int main(int argc, char *argv[]){   
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    // PIPES
    int keyboard_server[2], server_keyboard[2];
    char args_format[80]="%d %d|%d %d";
    sscanf(argv[1], args_format,  &keyboard_server[0], &keyboard_server[1], &server_keyboard[0], &server_keyboard[1]);
    


    pid_t keyboard_pid;
    keyboard_pid=getpid();
    my_write(keyboard_server[1], &keyboard_pid, server_keyboard[0],sizeof(keyboard_pid));

    // SGINAL

    int key;
    int xy[2] = {0,0};
    struct shared_data data;
    
    
    while (1) {
        // reads the position and user input from window
        printf("%d\n", keyboard_pid);
        my_read(server_keyboard[0], &data, server_keyboard[1],sizeof(data));
        key=data.key;
        
        switch ((char)key) {
            case ' ': // enter space to exit
            close(keyboard_server[1]);
            close(server_keyboard[0]);
                exit(EXIT_SUCCESS);

            // RIGHT 
            case 'o':
            case 'r':
                xy[0]++;
                xy[1]--;
                break;
            case 'l':
            case 'f':
                xy[0]++;
                break;

            case '.':
            case 'v':
                xy[0]++;
                xy[1]++;
                break;

            // LEFT
            case 'u':
            case 'w':
                xy[0]--;
                xy[1]--;
                break;
            case 'j':
            case 's':
                xy[0]--;
                break;
            case 'm':
            case 'x':
                xy[0]--;
                xy[1]++;
                break;
            
            // UP
            case 'i':
            case 'e':
                xy[1]--;
                break;

            // DOWN
            case ',':
            case 'c':
                xy[1]++;
                break;


            // STOP
            case 'k':
            case 'd':
                xy[0]=0;
                xy[1]=0;
            default:
                break;
        }


        // 3 Send the command force to drone.c
        memcpy(data.command_force,xy,sizeof(xy));
        my_write(keyboard_server[1], &data, server_keyboard[0],sizeof(data));

    }
    close(keyboard_server[1]);
    close(server_keyboard[0]);
    close(keyboard_server[0]); 
    close(server_keyboard[1]);

    return 0;
}