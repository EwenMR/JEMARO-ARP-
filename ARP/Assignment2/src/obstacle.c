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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SCREEN_WIDTH  10
#define SCREEN_HEIGHT 10
#define MIN_DISAPPEARANCE_DURATION 2 // Minimum disappearance duration in seconds
#define MAX_DISAPPEARANCE_DURATION 7 // Maximum disappearance duration in seconds

typedef struct {
    int x;
    int y;
    time_t timestamp; // Timestamp when the obstacle was created
    time_t disappearanceTime; // Time when the obstacle will disappear
} Obstacle;

void placeObstacle(Obstacle *obstacle) {
    obstacle->x = rand() % SCREEN_WIDTH + 1;
    obstacle->y = rand() % SCREEN_HEIGHT + 1;
    obstacle->timestamp = time(NULL);
    obstacle->disappearanceTime = obstacle->timestamp + (rand() % (MAX_DISAPPEARANCE_DURATION - MIN_DISAPPEARANCE_DURATION + 1) + MIN_DISAPPEARANCE_DURATION);
}

void updateObstacle(Obstacle *obstacle, time_t currentTime) {
    if (currentTime >= obstacle->disappearanceTime) {
        placeObstacle(obstacle);
    }
}

int main() {
    srand(time(NULL)); // Seed the random number generator with current time

    Obstacle obstacle;
    placeObstacle(&obstacle);

    time_t startTime = time(NULL);

    while (time(NULL) - startTime <= MAX_DISAPPEARANCE_DURATION) {
        updateObstacle(&obstacle, time(NULL));

        // You can perform other game logic here

        usleep(500000); // Sleep for 500 milliseconds (adjust as needed)
    }

    return 0;
}
