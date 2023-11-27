# INC=-I headers
all:  master server drone keyboard window watchdog test

# clean:
# 	rm build/*

master: 
	gcc master.c -o build/master 

server: 
	gcc server.c -o build/server -lncurses

drone: 
	gcc drone.c -o build/drone -lncurses

keyboard: 
	gcc keyboardManager.c -o build/keyboard 

window: 
	gcc window.c -o build/window -lncurses

watchdog: 
	gcc watchdog.c -o build/watchdog

test: 
	gcc test.c -o build/test -lncurses

save: 
	gcc save.c -o build/save -lncurses

# server: 
# 	gcc $(INC) server.c -o server -lncurses

