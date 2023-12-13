#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>


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


int main(int argc, char* argv[]){
    // SIGNALS
    struct sigaction signal;
    signal.sa_sigaction = signal_handler;
    signal.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &signal, NULL);
    sigaction(SIGUSR1, &signal, NULL);

    // PIPES
    int obstacle_server[2], server_obstacle[2];
    char args_format[80]="%d %d|%d %d";
    sscanf(argv[1], args_format,  &obstacle_server[0], &obstacle_server[1], &server_obstacle[0], &server_obstacle[1]);
    printf("%d %d %d %d\n",  obstacle_server[0], obstacle_server[1], server_obstacle[0], server_obstacle[1]);
    close(obstacle_server[0]); //Close unnecessary pipes
    close(server_obstacle[1]);

    pid_t obstacle_pid;
    obstacle_pid=getpid();
    write(obstacle_server[1], &obstacle_pid, sizeof(obstacle_pid));

    while(1){

    }

}





//stuff for inspo

#define NUM_OBSTACLES 10

typedef struct {
    double x;  // Change the type to float or double
    double y;  // Change the type to float or double
} Obstacle;

void printObstacles(Obstacle obstacles[]) {
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        printf("Obstacle %d: x = %.3f, y = %.3f\n", i + 1, obstacles[i].x, obstacles[i].y);
    }
}

void generateObstacles(Obstacle obstacles[]) {
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i].x = ((double)rand() / RAND_MAX) * 100.0;  // Random value between 0 and 100
        obstacles[i].y = ((double)rand() / RAND_MAX) * 100.0;  // Random value between 0 and 100
    }
}

void sendObstacles(int pipe_fd, Obstacle obstacles[]) {
    write(pipe_fd, obstacles, sizeof(Obstacle) * NUM_OBSTACLES);
}

int main() {
    srand(time(NULL));

    while (1) {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }

        Obstacle obstacles[NUM_OBSTACLES];

        generateObstacles(obstacles);
        // sendObstacles(pipe_fd[1], obstacles);

        printObstacles(obstacles);
        sleep(rand() % 6 + 5);  // Sleep for 5 to 10 seconds

        // generateObstacles(obstacles);
        // sendObstacles(pipe_fd[1], obstacles);

        close(pipe_fd[0]);
        close(pipe_fd[1]);
    }

    return 0;
}
