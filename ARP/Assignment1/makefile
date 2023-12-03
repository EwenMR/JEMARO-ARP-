# INC=-I headers
all:  master server drone keyboard window watchdog 

# clean:
# 	rm build/*

run:
	./build/master

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
	gcc watchdog.c -o build/watchdog -lncurses


# server: 
# 	gcc $(INC) server.c -o server -lncurses

