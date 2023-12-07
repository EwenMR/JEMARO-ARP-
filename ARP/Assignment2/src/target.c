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
    int target_server[2], server_target[2];
    char args_format[80]="%d %d|%d %d";
    sscanf(argv[1], args_format,  &target_server[0], &target_server[1], &server_target[0], &server_target[1]);
    printf("%d %d|%d %d\n",  target_server[0],   target_server[1],   server_target[0],   server_target[1]);
    close(target_server[0]); //Close unnecessary pipes
    close(server_target[1]);

    pid_t target_pid;
    target_pid=getpid();
    write(target_server[1], &target_pid, sizeof(target_pid));

    while(1){

    }

}




// for INSPO

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define TARGET_COUNT 9

typedef struct {
    int x;
    int y;
    int value;
    int collected;
} Target;

void placeTarget(Target *target, int droneX, int droneY) {
    do {
        target->x = rand() % 10 + 1; // Random x-coordinate
        target->y = rand() % 10 + 1; // Random y-coordinate
    } while (target->x == droneX && target->y == droneY);

    target->value = rand() % TARGET_COUNT + 1;
    target->collected = 0;
}

int updateTarget(Target *target, int droneX, int droneY, int currentTarget) {
    if (!target->collected && droneX == target->x && droneY == target->y) {
        if (target->value == currentTarget) {
            target->collected = 1;
            return 1; // Target collected in the correct order
        } else {
            return -1; // Target collected, but in the wrong order
        }
    }
    return 0; // Target not collected
}

int main() {
    srand(time(NULL)); // Seed the random number generator with current time

    int droneX = 5; // Initial drone x-coordinate
    int droneY = 5; // Initial drone y-coordinate

    Target targets[TARGET_COUNT];
    for (int i = 0; i < TARGET_COUNT; i++) {
        placeTarget(&targets[i], droneX, droneY);
    }

    int currentTarget = 1;

    while (currentTarget <= TARGET_COUNT) {
        int result = updateTarget(&targets[currentTarget - 1], droneX, droneY, currentTarget);

        if (result == 1) {
            printf("Target %d collected!\n", currentTarget);
            currentTarget++;
        } else if (result == -1) {
            printf("Wrong order! Target %d collected out of order.\n", targets[currentTarget - 1].value);
            // Handle wrong order logic here if needed
        }

        // You can perform other game logic here

        usleep(500000); // Sleep for 500 milliseconds (adjust as needed)
    }

    printf("All targets collected in the correct order!\n");

    return 0;
}
