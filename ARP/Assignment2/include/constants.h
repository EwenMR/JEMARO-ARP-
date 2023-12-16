#ifndef CONSTANTS_H
#define CONSTANTS_H



#define KEY_POS 2594

#define MAX_MSG_LEN 300
#define MAX_LOGGER_LEN 100

#define SEM_PATH "/my_sem_path"     // Customize the path
#define SHM_PATH "/my_shm_path"

#define SHM_SIZE sizeof(struct Position)

#define M 1.0
#define K 1.0
#define T 1

#define BOARD_SIZE 100
#define NUM_PROCESSES 7
#define NUM_OBSTACLES 10
#define NUM_TARGETS 9

#define WINDOW_COL 0.99
#define SCORE_WINDOW_ROW 0.20
#define WINDOW_ROW 0.80

struct shared_data{
    double drone_pos[6]; // Array to store the position of drone
    double obst_pos[NUM_OBSTACLES*2];
    double target_pos[NUM_TARGETS*2];
    int key;
    int command_force[2];
};

#endif