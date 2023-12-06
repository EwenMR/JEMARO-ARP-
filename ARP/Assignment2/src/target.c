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