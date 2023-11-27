#include "include/shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
<<<<<<< HEAD
#include <semaphore.h>
#include <sys/mman.h>
#include "include/constants.h"

#include <unistd.h>

// Define a semaphore
sem_t *sem_id;

int getSharedMemorySegment(key_t key, size_t size) {
    int shmfd = shm_open(SHMOBJ_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd == -1) {
=======
#include <sys/mman.h>
#include <unistd.h> 

int createSharedMemory() {
    int shmid = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmid == -1) {
>>>>>>> master
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

<<<<<<< HEAD
    ftruncate(shmfd, size);

    int shmid = shmfd; // Use file descriptor as the identifier
    return shmid;
}

struct Position *attachSharedMemory(int shmid) {
    // Wait for the semaphore before attaching
    sem_wait(sem_id);

    struct Position *ptr = mmap(NULL, sizeof(struct Position), PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void detachSharedMemory(struct Position *ptr) {
    // Detach from shared memory
    if (munmap(ptr, sizeof(struct Position)) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    // Post the semaphore after detaching
    sem_post(sem_id);
}

// Initialize the semaphore
void initSemaphore() {
    sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (sem_id == SEM_FAILED) {
        perror("sem_open");
=======
    if (ftruncate(shmid, sizeof(struct SharedData)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    return shmid;
}

struct SharedData *attachSharedMemory(int shmid) {
    struct SharedData *shared_data = (struct SharedData *)mmap(NULL, sizeof(struct SharedData),
                                                               PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return shared_data;
}

void detachSharedMemory(struct SharedData *shared_data) {
    if (munmap(shared_data, sizeof(struct SharedData)) == -1) {
        perror("munmap");
>>>>>>> master
        exit(EXIT_FAILURE);
    }
}

sem_t *createSemaphore() {
    sem_t *semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    return semaphore;
}

void cleanupSemaphore(sem_t *semaphore) {
    sem_close(semaphore);
    sem_unlink(SEMAPHORE_NAME);
}
