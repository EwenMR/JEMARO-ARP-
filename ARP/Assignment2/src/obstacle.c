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
    // close(obstacle_server[0]); //Close unnecessary pipes
    close(server_obstacle[1]);

    pid_t obstacle_pid;
    obstacle_pid=getpid();
    write(obstacle_server[1], &obstacle_pid, sizeof(obstacle_pid));
    printf("%d\n",obstacle_pid);

    while(1){

    }

}





//stuff for inspo



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define THRESHOLD 5.0   // threshold distance for the closeness an obstacle canbe to the drone
#define NUM_OBS 10      // number of obstacles
#define MAX_OBST_ARR_SIZE  20

// struct for obstacle x and y coordinates
typedef struct {
    double x;  
    double y;  
} Obstacle;


// printing the obstacle coordinates for checking values
void printObs(Obstacle obs[]) {
    for (int i = 0; i < NUM_OBS; i++) {
        printf("Obstacle %d: x = %.3f, y = %.3f\n", i + 1, obs[i].x, obs[i].y);
    }
}


void makeObs(Obstacle obs[], double droneX, double droneY) {
    for (int i = 0; i < NUM_OBS; i++) {
        // generating obstacle coordinates
        obs[i].x = ((double)rand() / RAND_MAX) * 500.0;  // Random value between 0 and 100
        obs[i].y = ((double)rand() / RAND_MAX) * 500.0;  // Random value between 0 and 100

        // check if the x obstacle coordinate isn't too close to drone
        while (obs[i].x >= droneX - THRESHOLD && obs[i].x <= droneX + THRESHOLD) {
            // Regenerate x-coordinate
            obs[i].x = ((double)rand() / RAND_MAX) * 500.0;
        }

        // check if the y obstacle coordinate isn't too close to drone
        while (obs[i].y >= droneY - THRESHOLD && obs[i].y <= droneY + THRESHOLD) {
            // Regenerate y-coordinate
            obs[i].y = ((double)rand() / RAND_MAX) * 500.0;
        }
    }
}






int main() {
    // seeds the random number generator
    srand(time(NULL));  

    // example values of the drone position, need to read the drone position from pipe instead
    double droneX = 50;
    double droneY = 50;

    // decalres 'obs' array of type Obstacle of size 10
    Obstacle obs[NUM_OBS];

    // generate first obstacle coordinates
    generateObstacles(obs, droneX, droneY);


    while (1) {
        // need to send Obstacles via pipe;

        // value checking 
        printObstacles(obs);

        // Sleep for 5 to 10 seconds
        sleep(rand() % 6 + 5);  

        generateObstacles(obs, droneX, droneY);
        
    }

    return 0;
}


