#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LOG_INFO "\x1b[32m[INFO] \x1b[0m"
#define LOG_ERROR "\x1b[31m[INFO] \x1b[0m"

// This is an alternative to sending the same message twice
bool checkMine(int id, int dest, char *string) {
    printf(LOG_INFO "I received string: %s, id = %d, dest = %d\n", string, id,
           dest);
    fflush(stdout);

    if (id == dest) {
        printf(LOG_INFO "And it's for me!\n\n");
        fflush(stdout);

        return true;
    }

    printf("But it's not for me so I send again: %s len: %lu\n\n", string,
           strlen(string) + 1);
    fflush(stdout);

    // Write again in fifo message received but not for me
    char *myfifo = "./myfifo";
    int sd = open(myfifo, O_WRONLY);
    write(sd, string, strlen(string) + 1);
    close(sd);

    return false;
}

int main(int argc, char *argv[]) {
    int fd1;
    char *myfifo = "./myfifo";
    char str1[80], str2[80];
    char format_string[80] = "%d|%d,%d";
    int n1, n2, dest;
    double mean;
    int id = argv[1][0] - '0';

    mkfifo(myfifo, 0666);

    printf(LOG_INFO "Im am child number %d\n\n", id);
    fflush(stdout);

    while (1) {
        fd1 = open(myfifo, O_RDONLY);
        if (!read(fd1, str1, 80)) {
            close(fd1);
            // this is busy waiting for some process to open the pipe in writing
            continue;
        }
        close(fd1);

        /* if the first input char is q, exit  */
        if (str1[0] == 'q') {
            exit(EXIT_SUCCESS);
        }

        /* read numbers from input line */
        if(sscanf(str1, format_string, &dest, &n1, &n2) != 3){
            printf(LOG_ERROR"Wrong string format\n");
            exit(EXIT_FAILURE);
        }

        if (!checkMine(id, dest, str1)) {
            continue;
        }

        mean = (n1 + n2) / 2.0;

        printf("mean value is: %f, sum is: %d\n\n", mean, n1 + n2);
        fflush(stdout);
    }

    return 0;
}
