#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <semaphore.h>

#define SHARED_MEMORY_NAME "/my_shared_memory"
#define SEMAPHORE_NAME "/my_semaphore"

// Define the shared data structure
struct SharedData {
    int data[3];
};

// Create or get the shared memory segment
int createSharedMemory();

// Attach to the shared memory segment
struct SharedData *attachSharedMemory(int shmid);

// Detach from the shared memory segment
void detachSharedMemory(struct SharedData *shared_data);

// Create or get the semaphore
sem_t *createSemaphore();

// Close and unlink the semaphore
void cleanupSemaphore(sem_t *semaphore);

#endif
