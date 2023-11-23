#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


bool check_target(int id, int target, int n, int m, char input_string[]){

    if (id == target){
        printf("This is my message\n");
        printf("received message is to child (%d), and the two numbers are %d, %d\n", target, n, m);
        fflush(stdout);
        return true;
    }
    printf("This message is not for me\n");
    printf("I will send it to the other child\n");

    char* myfifo = "./myfifo";
    mkfifo(myfifo, 0666);
    int fd = open(myfifo, O_WRONLY);
    
    write(fd,input_string,strlen(input_string)+1);
    close(fd);
    unlink(myfifo);

    return false;
}

int main(int argc, char* argv[]){
    int target, n,m;
    char receive_string[80];
    char format_string[80] = "%d|%d,%d";
    int id = argv[1][0] - '0';
    int fd;

    printf("This is child %d\n",id);

    while(1){
        char* myfifo = ("./myfifo");
        mkfifo(myfifo,0666);

        fd = open(myfifo, O_RDONLY);
        read(fd,receive_string,80);
        sscanf(receive_string,format_string,&target,&n,&m);


        if (receive_string[0]=='q'){
            close(fd);
            unlink(myfifo);
            exit(EXIT_SUCCESS);
        }
        
        if(sscanf(receive_string, format_string, &target, &n, &m) != 3){
            printf("Wrong string format\n");
            exit(EXIT_FAILURE);
        }
        if(check_target(id, target, n,m,receive_string)){
            printf("Mean is %d, Sum is %d\n", (n+m)/2, n+m);
            fflush(stdout);
        }

        close(fd);
        unlink(myfifo);


    }
    return 0;







}

// #include <fcntl.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <unistd.h>

// #define LOG_INFO "\x1b[32m[INFO] \x1b[0m"
// #define LOG_ERROR "\x1b[31m[INFO] \x1b[0m"

// // This is an alternative to sending the same message twice
// bool checkMine(int id, int dest, char *string) {
//     printf(LOG_INFO "I received string: %s, id = %d, dest = %d\n", string, id,
//            dest);
//     fflush(stdout);

//     if (id == dest) {
//         printf(LOG_INFO "And it's for me!\n\n");
//         fflush(stdout);

//         return true;
//     }

//     printf("But it's not for me so I send again: %s len: %lu\n\n", string,
//            strlen(string) + 1);
//     fflush(stdout);

//     // Write again in fifo message received but not for me
//     char *myfifo = "./myfifo";
//     int sd = open(myfifo, O_WRONLY);
//     write(sd, string, strlen(string) + 1);
//     close(sd);

//     return false;
// }

// int main(int argc, char *argv[]) {
//     int fd1;
//     char *myfifo = "./myfifo";
//     char str1[80], str2[80];
//     char format_string[80] = "%d|%d,%d";
//     int n1, n2, dest;
//     double mean;
//     int id = argv[1][0] - '0';

//     mkfifo(myfifo, 0666);

//     printf(LOG_INFO "Im am child number %d\n\n", id);
//     fflush(stdout);

//     while (1) {
//         fd1 = open(myfifo, O_RDONLY);
//         if (!read(fd1, str1, 80)) {
//             close(fd1);
//             // this is busy waiting for some process to open the pipe in writing
//             continue;
//         }
//         close(fd1);

//         /* if the first input char is q, exit  */
//         if (str1[0] == 'q') {
//             exit(EXIT_SUCCESS);
//         }

//         /* read numbers from input line */
//         if(sscanf(str1, format_string, &dest, &n1, &n2) != 3){
//             printf(LOG_ERROR"Wrong string format\n");
//             exit(EXIT_FAILURE);
//         }

//         if (!checkMine(id, dest, str1)) {
//             continue;
//         }

//         mean = (n1 + n2) / 2.0;

//         printf("mean value is: %f, sum is: %d\n\n", mean, n1 + n2);
//         fflush(stdout);
//     }

//     return 0;
// }
