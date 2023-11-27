#include <ncurses.h>
// window.c
#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include "include/constants.h"
#include "shared_memory.c"


WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void ncursesSetup(WINDOW **display, WINDOW **score)
{
    int initPos[4] = {
        LINES/200,
        COLS/200,
        (LINES/200) + LINES - (LINES/5),
        COLS/200
    };

    *score = create_newwin(LINES / 5, COLS - (COLS/100), initPos[2], initPos[3]);
    *display = create_newwin(LINES - (LINES/5), COLS - (COLS/100), initPos[0], initPos[1]);
    
    // mvwprintw(*display,1,2,"O");
    // wrefresh(*display);
    wrefresh(*display);
    wrefresh(*score);
}

int main() {
<<<<<<< HEAD
    initscr();
    cbreak();
    // WINDOW *win, *score;
    // ncursesSetup(&win, &score);

    struct Position *drone_pos = malloc(sizeof(struct Position));

    

    drone_pos->x = COLS/2;
    drone_pos->y = LINES/2;

    int counter=0;
    while (1) {
        WINDOW *win, *score;
        ncursesSetup(&win, &score);
        if (counter==0){
            mvwprintw(win,drone_pos->y,drone_pos->x,"X");
            counter++;
        }else{
            mvwprintw(win,drone_pos->y,drone_pos->x,"O");
        }
        wrefresh(win);
        
        drone_pos->key = wgetch(win);

        if (drone_pos->key=='o' || drone_pos->key=='l' || drone_pos->key=='.' || drone_pos->key=='r' || drone_pos->key=='f' || drone_pos->key=='v'){ //right or diagonalRight
            drone_pos->x=drone_pos->x+1;
        }if(drone_pos->key=='u'|| drone_pos->key=='j' || drone_pos->key=='m' || drone_pos->key=='w' || drone_pos->key=='s' || drone_pos->key=='x'){ //left or diagonalLeft
            drone_pos->x=drone_pos->x-1;
        }if(drone_pos->key == 'u'|| drone_pos->key=='i' || drone_pos->key=='o' || drone_pos->key=='w' || drone_pos->key=='e' || drone_pos->key=='r'){ //up
            drone_pos->y=drone_pos->y-1;
        }if(drone_pos->key == 'm'|| drone_pos->key==',' || drone_pos->key=='.' || drone_pos->key=='x' || drone_pos->key=='c' || drone_pos->key=='v') // down
            drone_pos->y=drone_pos->y+1;

        wclear(win);
        
        
    }

=======
    // Initialize ncurses
    initscr();
    // Don't display cursor
    curs_set(0);

    int position[6];
    int shared_seg_size = (1 * sizeof(position));

    sem_t * sem_id = sem_open(SEM_PATH, 0);

    int shmfd  = shm_open(SHM_PATH, O_RDONLY, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);


    while (1) {

        sem_wait(sem_id);
        // copy memory to local cells
        memcpy(position, shm_ptr, shared_seg_size);
        // post semaphore as soon as we are done
        sem_post(sem_id);

        int row, col;
        getmaxyx(stdscr, row, col);
        mvprintw(row / 2, (col - 5) / 2, "hello %d %d %d",position[0],position[1],position[2]);

        // Refresh the screen
        refresh();
    }

    
    shm_unlink(SHM_PATH);
    sem_close(sem_id);
    sem_unlink(SEM_PATH);


    // Print "hello" in the center of the screen
    

    // End ncurses
>>>>>>> master
    endwin();

    return 0;
}





// #include <curses.h>

// int main() {
//     WINDOW *mywin;

//     // Initialize the curses library
//     if (initscr() == NULL) {
//         fprintf(stderr, "Error initializing curses.\n");
//         return 1;
//     }

//     cbreak();  // Disable line buffering
//     noecho();  // Don't display typed characters

//     // Create a new window
//     mywin = newwin(10, 20, 0, 0);
//     if (mywin == NULL) {
//         fprintf(stderr, "Error creating window.\n");
//         endwin(); // End curses mode
//         return 1;
//     }

//     box(mywin, 0, 0); // Add a box around the window
//     wrefresh(mywin); // Refresh the window to display the box

//     int ch;
//     while ((ch = wgetch(mywin)) != 'q') { // Loop until 'q' is pressed
//         // Do something with the input, for example, move the window
//         switch (ch) {
//             case KEY_UP:
//                 mvwin(mywin, getbegy(mywin) - 1, getbegx(mywin));
//                 break;
//             case KEY_DOWN:
//                 mvwin(mywin, getbegy(mywin) + 1, getbegx(mywin));
//                 break;
//             case KEY_LEFT:
//                 mvwin(mywin, getbegy(mywin), getbegx(mywin) - 1);
//                 break;
//             case KEY_RIGHT:
//                 mvwin(mywin, getbegy(mywin), getbegx(mywin) + 1);
//                 break;
//         }
//         wrefresh(mywin); // Refresh the window after moving
//     }

//     endwin(); // End curses mode

//     return 0;
// }


// window and getch() doesnt go along