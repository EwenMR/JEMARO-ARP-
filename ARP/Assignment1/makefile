# INC=-I headers
all:  clean master server drone keyboard window watchdog

clean:
	rm build/*

master: 
	gcc src/master.c -o build/master 

server: 
	gcc src/server.c -o build/server

drone: 
	gcc src/drone.c -o build/drone 

keyboard: 
	gcc src/keyboardManager.c -o build/keyboard 

window: 
	gcc src/window.c -o build/window -lncurses

watchdog: 
	gcc src/watchdog.c -o build/watchdog 

run:
	./build/master

