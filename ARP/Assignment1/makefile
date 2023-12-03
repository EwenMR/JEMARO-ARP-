# INC=-I headers
all:  clean master server drone keyboard window watchdog

clean:
	rm build/*

master: 
	gcc master.c -o build/master 

server: 
	gcc server.c -o build/server

drone: 
	gcc drone.c -o build/drone 

keyboard: 
	gcc keyboardManager.c -o build/keyboard 

window: 
	gcc window.c -o build/window -lncurses

watchdog: 
	gcc watchdog.c -o build/watchdog 

run:
	./build/master

