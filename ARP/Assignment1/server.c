#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#define MAX_MSG_LEN 300
#define MAX_LOGGER_LEN 100

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);

    return local_win;
}

void ncursesSetup(WINDOW **display, WINDOW **logger)
{
    int initPos[2] = {
        LINES/200,
        COLS/200
    };

    *display = create_newwin(LINES - (LINES/100), COLS - (COLS/100), initPos[0], initPos[1]);
    // *display = create_newwin(30,40,0,0);
    // *logger = create_newwin(LINES / 2, COLS / 2, initPos[0], initPos[1]);
}



// void loggerAdd(int *loggerDim, WINDOW **logger, char loggerStrings[MAX_LOGGER_LEN][MAX_MSG_LEN], const char string[MAX_MSG_LEN])
// {
//     if (*loggerDim < MAX_LOGGER_LEN)
//         (*loggerDim)++;
//     for (int i = *loggerDim; i >= 0; i--)
//     {
//         // dest - src
//         strcpy(loggerStrings[i + 1], loggerStrings[i]);
//     }

//     strcpy(loggerStrings[0], string);

//     for (int i = 0; i < *loggerDim && i < getmaxy(*logger) - 2; i++)
//     {
//         mvwprintw(*logger, i + 1, (getmaxx(*logger) - strlen(string)) / 2, "%s", loggerStrings[i]);
//     }
//     wrefresh(*logger);
// }

int main(int argc, char *argv[])
{
    // ncurses stuff
    // char loggerStrings[MAX_LOGGER_LEN][MAX_MSG_LEN];
    // char logmsg[MAX_MSG_LEN + 30]; // it needs to accommodate also strings like "RECEIVED"
    // int loggerDim = 0;
    initscr();

    int x,y,key;
    // getmaxyx(stdscr,y,x);
    // x = y = x/2;
    x=COLS/2;
    y=LINES/2;

    
    // cbreak();

    WINDOW *display, *logger;
    ncursesSetup(&display, &logger);
    ncursesSetup(&display, &logger);
    // printw("hello");


    char receivedMsg[MAX_MSG_LEN];
    char toSend[MAX_MSG_LEN];
    while (true)
    {
        // initscr();
        move(y,x);
        printw("X");
        refresh();
        key = getch();
        // printw("%c", key);


        if (key=='o' || key=='l' || key=='.' || key=='r' || key=='f' || key=='v'){ //right or diagonalRight
            x=x+1;
        }if(key=='u'|| key=='j' || key=='m' || key=='w' || key=='s' || key=='x'){ //left if diagonalLeft
            x=x-1;
        }if(key == 'u'|| key=='i' || key=='o' || key=='w' || key=='e' || key=='r'){ //up
            y=y-1;
        }if(key == 'm'|| key==',' || key=='.' || key=='x' || key=='c' || key=='v') // down
            y=y+1;
        // endwin();
        clear();
    }

    endwin();
    return EXIT_SUCCESS;
}
