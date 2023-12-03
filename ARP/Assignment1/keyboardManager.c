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


#include "include/constants.h"

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
    }
}

int main(int argc, char *argv[])

{   
    
    // PIPES
    int keyboard_drone[2],window_keyboard[2], wd_keyboard[2],key_wd[2];
    pid_t keyboard_pid,wd_pid;
    int dummy;
    keyboard_pid=getpid();
    sscanf(argv[1],"%d %d|%d %d|%d %d", &window_keyboard[0], &window_keyboard[1], 
                                        &keyboard_drone[0], &keyboard_drone[1],
                                        &key_wd[0], &key_wd[1]);
    // close unnecessary pipes

    close(window_keyboard[1]); // fd of write to window
    close(keyboard_drone[0]);  // fd of read from drone
    // close(key_wd[1]);
    printf("%d\n", keyboard_pid);

    // read(key_wd[0], &wd_pid, sizeof(wd_pid));
    // printf("%d\n", wd_pid);


    close(key_wd[0]);

    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);
    int key;
    int xy[2] = {0,0};
    
    
    while (1) {
        // reads the position and user input from window
        int ret = read(window_keyboard[0], &key, sizeof(key));
        if (ret<0){
            perror("reading error\n");
            close(keyboard_drone[1]);
            close(window_keyboard[0]);
            exit(EXIT_FAILURE);
        }
        switch ((char)key) {
            case ' ': // enter space to exit
                close(window_keyboard[0]);
                close(keyboard_drone[1]);
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

            default:
                break;
        }


        // 3
        // Send the command force to drone.c
        int ret2=write(keyboard_drone[1], xy, sizeof(xy));
        if (ret2<0){
            close(window_keyboard[0]);
            close(keyboard_drone[1]);
            perror("writing error\n");
            exit(EXIT_FAILURE);
        }
    }
    close(window_keyboard[0]);
    close(keyboard_drone[1]);

    return 0;
}