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