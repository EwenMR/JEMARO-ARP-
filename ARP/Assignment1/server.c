#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <unistd.h> 
#include <stdlib.h>
#include "include/constants.h"
#include <semaphore.h>
#include <sys/mman.h>

#include <signal.h>

// Signal handler for watchdog
void signal_handler(int signo, siginfo_t *siginfo, void *context){
    if(signo == SIGINT){
        exit(1);
    }
    if(signo == SIGUSR1){
        pid_t wd_pid = siginfo->si_pid;
        kill(wd_pid, SIGUSR2);
    }
}

int main(int argc, char *argv[]) 
{
    // SIGNALS FOR THE WATCHDOG
    struct sigaction sig_act;
    sig_act.sa_sigaction = signal_handler;
    sig_act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sig_act, NULL);
    sigaction(SIGUSR1, &sig_act, NULL);

    

    // SENDING THE PID TO WATCHDOG
    pid_t server_pid,wd_pid;
    server_pid=getpid();
    int wd_server[2];
    sscanf(argv[1],"%d %d", &wd_server[0], &wd_server[1]); // Get the fds of the pipe to watchdog

    close(wd_server[0]);
    write(wd_server[1], &server_pid, sizeof(server_pid));  // Send the pid of the server to watchdog
    printf("%d\n",server_pid);
    close(wd_server[1]);
    
    
    
    
    // SHARED MEMORY AND SEMAPHORE
    double position[6]; // Array to store the position of drone
    int shared_seg_size = (sizeof(position));

    sem_t * sem_id = sem_open(SEM_PATH, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_init(sem_id, 1, 0); //initialized sem_id to 0 until shared memory is instantiated

    int shmfd  = shm_open(SHM_PATH, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG); // create shared memory object
    if (shmfd < 0){
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shmfd, shared_seg_size);  // truncate size of shared memory
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0); // map pointer
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    sem_post(sem_id); // post semaphore, sem_id=1

    while (1) 
    {      
        // copy the position of the drone from shared memory
        sem_wait(sem_id);
        memcpy(position, shm_ptr, shared_seg_size);
        sem_post(sem_id);

    } 

    // clean up
    shm_unlink(SHM_PATH);
    sem_close(sem_id);
    sem_unlink(SEM_PATH);
    munmap(shm_ptr, shared_seg_size);

    return 0; 
} 