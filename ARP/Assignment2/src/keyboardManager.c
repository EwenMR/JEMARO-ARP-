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

#include "../include/log.c"

#include "../include/constants.h"

void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        writeToLogFile(keyboardlogpath,"killed");
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
        writeToLogFile(keyboardlogpath,"signal received");
    }
}

int main(int argc, char *argv[]){   
    //SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);
    clearLogFile(keyboardlogpath);

    // PIPES
    int keyboard_server[2], server_keyboard[2];
    sscanf(argv[1], args_format,  &keyboard_server[0], &keyboard_server[1], &server_keyboard[0], &server_keyboard[1]);
    close(keyboard_server[0]); 
    close(server_keyboard[1]);
    

    // Pids for Watchdog
    pid_t keyboard_pid;
    keyboard_pid=getpid();
    printf("Keyboard PID: %d\n", keyboard_pid);
    my_write(keyboard_server[1], &keyboard_pid, server_keyboard[0],sizeof(keyboard_pid));
    writeToLogFile(keyboardlogpath, "KEYBOARD: Pid sent to server");


    // Local variables
    int key;
    int xy[2] = {0,0};
    struct shared_data data;
    
    
    while (1) {
        // Receive user input from window
        my_read(server_keyboard[0], &data, server_keyboard[1],sizeof(data));
        key=data.key;
        writeToLogFile(keyboardlogpath, "KEYBOARD: User input received from server");

        // Get command force
        switch ((char)key) {
            case ' ': // enter space to exit
            close(keyboard_server[1]);
            close(server_keyboard[0]);
                exit(EXIT_SUCCESS);

            // RIGHT 
            case 'o':
            case 'r':
            case '9':
                xy[0]++;
                xy[1]--;
                break;
            case 'l':
            case 'f':
            case '6':
                xy[0]++;
                break;

            case '.':
            case 'v':
            case '3':
                xy[0]++;
                xy[1]++;
                break;

            // LEFT
            case 'u':
            case 'w':
            case '7':
                xy[0]--;
                xy[1]--;
                break;
            case 'j':
            case 's':
            case '4':
                xy[0]--;
                break;
            case 'm':
            case 'x':
            case '1':
                xy[0]--;
                xy[1]++;
                break;
            
            // UP
            case 'i':
            case 'e':
            case '8':
                xy[1]--;
                break;

            // DOWN
            case ',':
            case 'c':
            case '2':
                xy[1]++;
                break;


            // STOP
            case 'k':
            case 'd':
            case '5':
                xy[0]=0;
                xy[1]=0;
            default:
                break;
        }


        // Send the command force to drone
        memcpy(data.command_force,xy,sizeof(xy));
        my_write(keyboard_server[1], &data, server_keyboard[0],sizeof(data));
        writeToLogFile(keyboardlogpath, "KEYBOARD:  Command force sent to server");

    }
    // cleanup
    close(keyboard_server[1]);
    close(server_keyboard[0]);
    

    return 0;
}