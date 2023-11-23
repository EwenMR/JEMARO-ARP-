#include "headers/constants.h"
#include "headers/wrapfunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

void summon(char **programArgs) {
    execvp("konsole", programArgs);
    perror("Execution failed");

    // avoid unwanted forking
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    char *argsServer[] = {"konsole",  "-e", "./server", "placeholder", NULL};
    char *argsWriter[] = {"konsole", "-e","./writer", "1","placeholder", NULL};
    char *argsReader[] = {"konsole", "-e","./reader", "1","placeholder", NULL};

    int server_reader1[2];
    int reader1_server[2];

    int reader2_server[2];
    int server_reader2[2];

    int server_writer1[2];
    int writer1_server[2];

    int server_writer2[2];
    int writer2_server[2];

    pipe(server_reader1);
    pipe(reader1_server);

    pipe(server_reader2);
    pipe(reader2_server);

    pipe(server_writer1);
    pipe(writer1_server);

    pipe(server_writer2);
    pipe(writer2_server);

    for (int i = 0; i < 5; i++) {
        pid_t pid = m_fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (!pid) {
            if (i == 0) {
                char args[MAX_MSG_LEN];
                sprintf(args, "%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d",
                        server_reader1[0], server_reader1[1], server_reader2[0],
                        server_reader2[1], server_writer1[0], server_writer1[1],
                        server_writer2[0], server_writer2[1], reader1_server[0],
                        reader1_server[1], reader2_server[0], reader2_server[1],
                        writer1_server[0], writer1_server[1], writer2_server[0],
                        writer2_server[1]);
                argsServer[3] = args;
                summon(argsServer);
            } else if (i % 2 == 0) {
                char args[MAX_MSG_LEN];
                if (i == 4){
                    argsWriter[3] = "2";
                    sprintf(args, "%d %d|%d %d", server_writer2[0], server_writer2[1], writer2_server[0], writer2_server[1]);
                }else{
                    sprintf(args, "%d %d|%d %d", server_writer1[0], server_writer1[1], writer1_server[0], writer1_server[1]);
                }
                argsWriter[4] = args;
                summon(argsWriter);
            } else {
                char args[MAX_MSG_LEN];
                if (i == 3){
                    argsReader[3] = "2";
                    sprintf(args, "%d %d|%d %d ", server_reader2[0], server_reader2[1], reader2_server[0], reader2_server[1]);
                }else{
                    sprintf(args, "%d %d|%d %d", server_reader1[0], server_reader1[1], reader1_server[0], reader1_server[1]);
                }
                argsReader[4] = args;
                summon(argsReader);
            }
        } else {
            printf("Summoned child with pid %d\n", pid);
        }
    }

    int stat;

    for (int i = 0; i < 5; i++) {
        pid_t pid = wait(&stat);
        printf("Child %d terminated with status %d\n", pid, stat);
    }
    return EXIT_SUCCESS;
}
