
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
    
    wrefresh(*display);
    wrefresh(*score);
}


int main(int argc, char* argv[]) {
    initscr();
    cbreak();

    int pos_key[3];

    // file descriptors for both writing and reading to keyboardManager
    int window_keyboard[2];
    int keyboard_window[2];
    pos_key[0] = COLS/2;
    pos_key[1] = LINES/2;
    sscanf(argv[1], "%d %d|%d %d", &window_keyboard[0],&window_keyboard[1], &keyboard_window[0], &keyboard_window[1]);
    close(window_keyboard[0]);
    close(keyboard_window[1]);

    int send_int = 1;
    int writer_num = -1;

   
    int position[4] = {0, 0, 0, 0};
    int shared_seg_size = (1 * sizeof(position));

    // create semaphore ids
    sem_t * sem_id = sem_open(SEM_PATH, 0);

    // create shared memory pointer
    int shmfd  = shm_open(SHM_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    if (shmfd < 0)
    {
        perror("shm_open");
        return -1;
    }
    void* shm_ptr = mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);


    while (1) {
        // refresh window
        WINDOW *win, *score;
        ncursesSetup(&win, &score);

        // move to the desired position and print "X", 
        mvwprintw(win, pos_key[1], pos_key[0], "X");
        wrefresh(win);

        // wait for user input
        pos_key[2]=wgetch(win);






        sem_wait(sem_id);
        // copy cells
        memcpy(position, shm_ptr, shared_seg_size);

        for(int i=0;i<3;i++){
            position[i]=pos_key[i];
        }
        // copy local cells to memory
        memcpy(shm_ptr, position, shared_seg_size);
    
        // post semaphore
        sem_post(sem_id);






        // writes the current position of drone and user input to keyboardManager
        int ret= write(window_keyboard[1], pos_key, sizeof(pos_key));
        if (ret<0){ 
            perror("writing error\n");
            exit(EXIT_FAILURE);
        }

        if((char)pos_key[2]==' '){
            close(window_keyboard[1]);
            close(keyboard_window[0]);
            exit(EXIT_SUCCESS);
        }

        // reads position of drone from keyboardManager
        int ret2 = read(keyboard_window[0], pos_key, sizeof(pos_key));
        if (ret2<0){
            perror("reading error\n");
            exit(EXIT_FAILURE);
        }

        clear();
    }

    
    shm_unlink(SHM_PATH);
    sem_close(sem_id);
    sem_unlink(SEM_PATH);

    endwin();

    return 0;
}