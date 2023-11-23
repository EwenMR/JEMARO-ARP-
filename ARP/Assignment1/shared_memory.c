// shared_memory.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "include/constants.h"

// Define a semaphore
sem_t *sem_id;

int getSharedMemorySegment(key_t key, size_t size) {
    if(key<0){
        perror("key");
    }
    if(size<0){
        perror("size");
    }
    
    int shmid = shmget(key, size, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        // exit(EXIT_FAILURE);
    }
    return shmid;
}

struct Position *attachSharedMemory(int shmid) {
    // Wait for the semaphore before attaching
    sem_wait(sem_id);
    
    return (struct Position *)shmat(shmid, NULL, 0);
}

void detachSharedMemory(struct Position *ptr) {
    // Detach from shared memory
    shmdt(ptr);
    
    // Post the semaphore after detaching
    sem_post(sem_id);
}

// Initialize the semaphore
void initSemaphore() {
    sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem_id == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
}

// Cleanup the semaphore
void cleanupSemaphore() {
    sem_close(sem_id);
    sem_unlink(SEM_PATH);
}
