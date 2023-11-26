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
    int keyboard_window[2];
    int window_keyboard[2];
    sscanf(argv[1],"%d %d|%d %d", &window_keyboard[0], &window_keyboard[1], &keyboard_window[0], &keyboard_window[1]);
    int pos_key[3];
    
    while (1) {
        int ret = read(window_keyboard[0], pos_key, sizeof(pos_key));
        if (ret<0){
            perror("reading error\n");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
        // char key=getch();

        if ((char)pos_key[2] == 'o' || (char)pos_key[2] == 'l' || (char)pos_key[2] == '.' || (char)pos_key[2] == 'r' || (char)pos_key[2] == 'f' || (char)pos_key[2] == 'v') {
            pos_key[0]++;
        }
        if ((char)pos_key[2] == 'u' || (char)pos_key[2] == 'j' || (char)pos_key[2] == 'm' || (char)pos_key[2] == 'w' || (char)pos_key[2] == 's' || (char)pos_key[2] == 'x') {
            pos_key[0]--;  // Incrementing for left, decrementing for right
        }
        if ((char)pos_key[2] == 'u' || (char)pos_key[2] == 'i' || (char)pos_key[2] == 'o' || (char)pos_key[2] == 'w' || (char)pos_key[2] == 'e' || (char)pos_key[2] == 'r') {
            pos_key[1]--;
        }
        if ((char)pos_key[2] == 'm' || (char)pos_key[2] == ',' || (char)pos_key[2] == '.' || (char)pos_key[2] == 'x' || (char)pos_key[2] == 'c' || (char)pos_key[2] == 'v') {
            pos_key[1]++;
        }
        int ret2=write(keyboard_window[1], pos_key, sizeof(pos_key));
        if (ret2<0){
            perror("writing error\n");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}