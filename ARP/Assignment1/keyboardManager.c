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
    int keyboard_window[2];
    int window_keyboard[2];
    sscanf(argv[1],"%d %d|%d %d", &window_keyboard[0], &window_keyboard[1], &keyboard_window[0], &keyboard_window[1]);

    close(window_keyboard[1]);
    close(keyboard_window[0]);

    int pos_key[3];
    
    while (1) {
        // reads the position and user input from window
        int ret = read(window_keyboard[0], pos_key, sizeof(pos_key));
        if (ret<0){
            perror("reading error\n");
            exit(EXIT_FAILURE);
        }
        if((char)pos_key[2]==' '){
            
        }
        switch ((char)pos_key[2]) {
            // enter space to exit
            case ' ':
                close(window_keyboard[0]);
                close(keyboard_window[1]);
                exit(EXIT_SUCCESS);

            // RIGHT 
            case 'o':
            case 'r':
                pos_key[0]++;
                pos_key[1]--;
                break;
            case 'l':
            case 'f':
                pos_key[0]++;
                break;

            case '.':
            case 'v':
                pos_key[0]++;
                pos_key[1]++;
                break;

            // LEFT
            case 'u':
            case 'w':
                pos_key[0]--;
                pos_key[1]--;
                break;
            case 'j':
            case 's':
                pos_key[0]--;
                break;
            case 'm':
            case 'x':
                pos_key[0]--;
                pos_key[1]++;
                break;
            
            // UP
            case 'i':
            case 'e':
                pos_key[1]--;
                break;

            // DOWN
            case ',':
            case 'c':
                pos_key[1]++;
                break;

            default:
                // Handle default case or do nothing
                break;
        }


        // if ((char)pos_key[2] == 'o' || (char)pos_key[2] == 'l' || (char)pos_key[2] == '.' || (char)pos_key[2] == 'r' || (char)pos_key[2] == 'f' || (char)pos_key[2] == 'v') {
        //     pos_key[0]++;
        // }
        // if ((char)pos_key[2] == 'u' || (char)pos_key[2] == 'j' || (char)pos_key[2] == 'm' || (char)pos_key[2] == 'w' || (char)pos_key[2] == 's' || (char)pos_key[2] == 'x') {
        //     pos_key[0]--;  // Incrementing for left, decrementing for right
        // }
        // if ((char)pos_key[2] == 'u' || (char)pos_key[2] == 'i' || (char)pos_key[2] == 'o' || (char)pos_key[2] == 'w' || (char)pos_key[2] == 'e' || (char)pos_key[2] == 'r') {
        //     pos_key[1]--;
        // }
        // if ((char)pos_key[2] == 'm' || (char)pos_key[2] == ',' || (char)pos_key[2] == '.' || (char)pos_key[2] == 'x' || (char)pos_key[2] == 'c' || (char)pos_key[2] == 'v') {
        //     pos_key[1]++;
        // }
        int ret2=write(keyboard_window[1], pos_key, sizeof(pos_key));
        if (ret2<0){
            perror("writing error\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}