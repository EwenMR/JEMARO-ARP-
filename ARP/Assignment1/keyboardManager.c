// keyboardManager.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/constants.h"
#include "shared_memory.c"




int main(int argc, char* argv[]){
    

    // Access shared memory data (drone_pos) here

    
    
    // printw(drone_pos);
    int key;

        // create semaphore ids
    initSemaphore();

    int shmid_pos = getSharedMemorySegment(KEY_POS, sizeof(struct Position));
    struct Position *drone_pos = attachSharedMemory(shmid_pos);
    printf("%d,%d", drone_pos->x, drone_pos->y);


    if (key=='o' || key=='l' || key=='.' || key=='r' || key=='f' || key=='v'){ //right or diagonalRight
        drone_pos->x=drone_pos->x+1;
    }if(key=='u'|| key=='j' || key=='m' || key=='w' || key=='s' || key=='x'){ //left or diagonalLeft
        drone_pos->x=drone_pos->x-1;
    }if(key == 'u'|| key=='i' || key=='o' || key=='w' || key=='e' || key=='r'){ //up
        drone_pos->y=drone_pos->y-1;
    }if(key == 'm'|| key==',' || key=='.' || key=='x' || key=='c' || key=='v') // down
        drone_pos->y=drone_pos->y+1;

    detachSharedMemory(drone_pos);

    
}
