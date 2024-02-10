#ifndef CONSTANTS_H
#define CONSTANTS_H



#define KEY_POS 2594

#define MAX_MSG_LEN 300
#define MAX_LOGGER_LEN 100


#define SHM_SIZE sizeof(struct Position)

#define M 1.0
#define K 1.0
#define T 1

#define p 5
#define n 25
#define p_att 20


#define BOARD_SIZE 100
#define NUM_PROCESSES 5
#define NUM_OBSTACLES 10
#define NUM_TARGETS 9

#define WINDOW_COL 0.99
#define SCORE_WINDOW_ROW 0.20
#define WINDOW_ROW 0.80

#define TARGET_THRESH 10          // threshold distance for the closeness an obstacle canbe to the drone
#define MAX_TAR_ARR_SIZE  20    // max array size for targets
#define POSITION_THRESHOLD 5.0

#define OBS_THRESH 5.0 
#define OBSTACLE_REFRESH_RATE 10

#define WD_TIMER_THRESH 3

struct shared_data{
    double drone_pos[6]; // Array to store the position of drone
    double obst_pos[NUM_OBSTACLES*2];
    double target_pos[NUM_TARGETS*2];
    int key;
    int command_force[2];
};

char *serverlogpath = "./log/server.log"; // Path for the log file
char *windowlogpath = "./log/window.log"; // Path for the log file
char *dronelogpath = "./log/drone.log"; // Path for the log file
char *obstaclelogpath = "./log/obstacle.log"; // Path for the log file
char *targetlogpath = "./log/target.log"; // Path for the log file
char *wdlogpath = "./log/watchdog.log"; // Path for the log file
char *keyboardlogpath = "./log/keyboard.log"; // Path for the log file
char server_format[100]= "%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d|%d %d";
char args_format[80]="%d %d|%d %d";

#endif