// keyboardManager.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "include/constants.h"
#include "shared_memory.c"

int main(int argc, char *argv[])
{
    // file descriptors for for both reading and writing
    int keyboard_drone[2];
    int window_keyboard[2];
    sscanf(argv[1],"%d %d|%d %d", &window_keyboard[0], &window_keyboard[1], &keyboard_drone[0], &keyboard_drone[1]);

    close(window_keyboard[1]);
    close(keyboard_drone[0]);

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
            // enter space to exit
            case ' ':
                // sends notification to drone
                close(window_keyboard[0]);
                close(keyboard_drone[1]);
                exit(EXIT_SUCCESS);

            // RIGHT 
            case 'o':
            case 'r':
                xy[0]=1;
                xy[1]=1;
                break;
            case 'l':
            case 'f':
                xy[0]=1;
                xy[1]=0;
                break;

            case '.':
            case 'v':
                xy[0]=1;
                xy[1]=-1;
                break;

            // LEFT
            case 'u':
            case 'w':
                xy[0]=-1;
                xy[1]=1;
                break;
            case 'j':
            case 's':
                xy[0]=-1;
                xy[1]=0;
                break;
            case 'm':
            case 'x':
                xy[0]=-1;
                xy[1]=-1;
                break;
            
            // UP
            case 'i':
            case 'e':
                xy[0]=0;
                xy[1]=1;
                break;

            // DOWN
            case ',':
            case 'c':
                xy[0]=0;
                xy[1]=-1;
                break;

            default:
                // Handle default case or do nothing
                break;
        }



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