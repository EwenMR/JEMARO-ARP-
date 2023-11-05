#include <ncurses.h>

int main() {
    initscr();
    printw("Hello, NCurses!");
    refresh();
    getch();
    endwin();
    return 0;
}
