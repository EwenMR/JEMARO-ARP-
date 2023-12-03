#ifndef CONSTANTS_H
#define CONSTANTS_H



#define KEY_POS 2594
#define NUM_GOALS 3
#define NUM_OBSTACLES 10

#define MAX_MSG_LEN 300
#define MAX_LOGGER_LEN 100

#define SEM_PATH "/my_sem_path"     // Customize the path
#define SHM_PATH "/my_shm_path"

#define SHM_SIZE sizeof(struct Position)

#define M 1.0
#define K 1.0
#define T 1
#define FORCEX 1.0
#define FORCEY 1.0

#define BOARD_SIZE 100

#define NUM_PROCESSES 5


struct Position {
    int xbefore;
    int ybefore;
    int xnow;
    int ynow;
};

#endif