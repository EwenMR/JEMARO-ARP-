#include <curses.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

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
    initscr();
    // cbreak();
    // WINDOW *win, *score;
    // ncursesSetup(&win, &score);

    struct Position *drone_pos = malloc(sizeof(struct Position));

    

    drone_pos->x = COLS/2;
    drone_pos->y = LINES/2;

    int counter=0;
    while (1) {
        if (counter==0){
            move(drone_pos->y,drone_pos->x);
            printw("X");
            counter++;
        }else{
            move(drone_pos->y,drone_pos->x);
            printw("O");
        }
        
        refresh();
        drone_pos->key = getch();
        if (drone_pos->key=='o' || drone_pos->key=='l' || drone_pos->key=='.' || drone_pos->key=='r' || drone_pos->key=='f' || drone_pos->key=='v'){ //right or diagonalRight
            drone_pos->x=drone_pos->x+1;
        }if(drone_pos->key=='u'|| drone_pos->key=='j' || drone_pos->key=='m' || drone_pos->key=='w' || drone_pos->key=='s' || drone_pos->key=='x'){ //left or diagonalLeft
            drone_pos->x=drone_pos->x-1;
        }if(drone_pos->key == 'u'|| drone_pos->key=='i' || drone_pos->key=='o' || drone_pos->key=='w' || drone_pos->key=='e' || drone_pos->key=='r'){ //up
            drone_pos->y=drone_pos->y-1;
        }if(drone_pos->key == 'm'|| drone_pos->key==',' || drone_pos->key=='.' || drone_pos->key=='x' || drone_pos->key=='c' || drone_pos->key=='v') // down
            drone_pos->y=drone_pos->y+1;

        clear();
        
    }

    endwin();

    return 0;
}
