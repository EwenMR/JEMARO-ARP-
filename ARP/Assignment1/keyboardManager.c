// keyboardManager.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/constants.h"
#include "shared_memory.c"




int main(int argc, char* argv[]){
    

    // Access shared memory data (drone_pos) here
    int shm_fd;
    struct Position *drone_pos;
    sem_t *semaphore;

    // Open the semaphore created by the producer
    semaphore = sem_open("/my_semaphore", 0);
    if (semaphore == SEM_FAILED) {
        perror("Error opening semaphore");
        exit(EXIT_FAILURE);
    }

    // Open the shared memory object created by the producer
    shm_fd = shm_open("/my_shared_memory", O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error opening shared memory");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory into the address space
    drone_pos = (struct Position *)mmap(NULL, sizeof(struct Position), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (drone_pos == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(EXIT_FAILURE);
    }
    
    
    // printw(drone_pos);
    // int key;

        // create semaphore ids
    // initSemaphore();

    // int shmid_pos = getSharedMemorySegment(KEY_POS, sizeof(struct Position));
    // struct Position *drone_pos = attachSharedMemory(shmid_pos);
    

    printf("%d,%d", drone_pos->x, drone_pos->y);
    while(1){
        sem_wait(semaphore);


        if (drone_pos->key=='o' || drone_pos->key=='l' || drone_pos->key=='.' || drone_pos->key=='r' || drone_pos->key=='f' || drone_pos->key=='v'){ //right or diagonalRight
            drone_pos->x=drone_pos->x+1;
        }if(drone_pos->key=='u'|| drone_pos->key=='j' || drone_pos->key=='m' || drone_pos->key=='w' || drone_pos->key=='s' || drone_pos->key=='x'){ //left or diagonalLeft
            drone_pos->x=drone_pos->x-1;
        }if(drone_pos->key == 'u'|| drone_pos->key=='i' || drone_pos->key=='o' || drone_pos->key=='w' || drone_pos->key=='e' || drone_pos->key=='r'){ //up
            drone_pos->y=drone_pos->y-1;
        }if(drone_pos->key == 'm'|| drone_pos->key==',' || drone_pos->key=='.' || drone_pos->key=='x' || drone_pos->key=='c' || drone_pos->key=='v') // down
            drone_pos->y=drone_pos->y+1;

        sem_post(semaphore);
    }

    if (munmap(drone_pos, sizeof(struct Position)) == -1) {
            perror("Error unmapping shared memory");
            exit(EXIT_FAILURE);
        }

    // Close shared memory object
    close(shm_fd);

    // Close the semaphore
    sem_close(semaphore);

    
}
