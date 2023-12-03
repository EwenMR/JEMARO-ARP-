#include "include/shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h> 

#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include "include/constants.h"

int createSharedMemory() {
    int shmid = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
    if (shmid == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shmid, sizeof(struct SharedData)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    return shmid;
}
sem_t * sem_id = sem_open(SEM_PATH, 0); // create semaphore ids
    int shmfd  = shm_open(SHM_PATH, O_RDWR, S_IRWXU | S_IRWXG); // create shared memory pointer
    if (shmfd < 0){ 
        perror("shm_open");
        return -1;
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0); // map a shared memory segment into the address space
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }



void detachSharedMemory(struct SharedData *shared_data) {
    if (munmap(shared_data, sizeof(struct SharedData)) == -1) {
        perror("munmap");
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