#include <stdio.h>
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
    // definition of process IDs
    pid_t child_writer0;
    pid_t child_writer1;
    pid_t child_reader0;
    pid_t child_reader1;
    pid_t child_server;

    int writer0_server[2];
    int server_writer0[2];
    int writer1_server[2];
    int server_writer1[2];
    int reader0_server[2];
    int server_reader0[2];
    int reader1_server[2];
    int server_reader1[2];

    pipe(writer0_server);
    pipe(server_writer0);
    pipe(writer1_server);
    pipe(server_writer1);
    pipe(reader0_server);
    pipe(server_reader0);
    pipe(reader1_server);  
    pipe(server_reader1);

    int res;
    int num_children;

    // list of parameters to pass the fd numbers to the executed process
    // child is a different process, so it doesnt know the pid. The father will have to pass the info
    char writer0_args[80];
    char writer1_args[80];
    char reader0_args[80];
    char reader1_args[80];

    // preparation of all the parameter strings
    // 2writer, 2reader, 1server
    sprintf(writer0_args, "%d %d %d %d", writer0_server[0], writer0_server[1], server_writer0[0], server_writer0[1]);
    sprintf(writer1_args, "%d %d %d %d", writer1_server[0], writer1_server[1], server_writer1[0], server_writer1[1]);
    sprintf(reader0_args, "%d %d %d %d", reader0_server[0], reader0_server[1], server_reader0[0], server_reader0[1]);
    sprintf(reader1_args, "%d %d %d %d", reader1_server[0], reader1_server[1], server_reader1[0], server_reader1[1]);

    // Now I start forking
    child_server = fork();
    // Error check, if child server is 0, fork returns -1 in case of failure
    if (child_server < 0) {
        perror("Fork");
        return -1;
    }

    // 'konsole' is not a necessary. -e can get the list of unused konsoles
    if (child_server == 0) {
        char * arg_list[] = { "konsole", "-e", "./server", writer0_args, writer1_args, reader0_args, reader1_args, NULL };
        execvp("konsole", arg_list);
	return 0;
    }
    num_children += 1;

    child_writer0 = fork();
    if (child_writer0 < 0) {
        perror("Fork");
        return -1;
    }

    if (child_writer0 == 0) {
        char * arg_list[] = { "konsole", "-e", "./writer", "0", writer0_args, NULL };
        execvp("konsole", arg_list);
	return 0;
    }
    num_children += 1;
    
    child_writer1 = fork();
    if (child_writer1 < 0) {
        perror("Fork");
        return -1;
    }

    if (child_writer1 == 0) {
        char * arg_list[] = { "konsole", "-e", "./writer", "1", writer1_args, NULL };
        execvp("konsole", arg_list);
	return 0;
    }
    num_children += 1;


    child_reader0 = fork();
    if (child_reader0 < 0) {
        perror("Fork");
        return -1;
    }

    if (child_reader0 == 0) {
        char * arg_list[] = { "konsole", "-e", "./reader", "0", reader0_args, NULL };
        execvp("konsole", arg_list);
	return 0;
    }
    num_children += 1;
    
    child_reader1 = fork();
    if (child_reader1 < 0) {
        perror("Fork");
        return -1;
    }

    if (child_reader1 == 0) {
        char * arg_list[] = { "konsole", "-e", "./reader", "1", reader1_args, NULL };
        execvp("konsole", arg_list);
	return 0;
    }
    num_children += 1;
    //wait for all children to terminate
    for(int i = 0; i < num_children; i ++){
        wait(&res);
    }

    return 0;
}
