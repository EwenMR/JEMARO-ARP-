# INC=-I headers
all:  clean master server drone keyboard window obstacle target watchdog  

clean:
	rm -rf build
	mkdir build

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

obstacle:
	gcc src/obstacle.c -o build/obstacle

target:
	gcc src/target.c -o build/target

watchdog: 
	gcc src/watchdog.c -o build/watchdog 

clear:
	clear
run:
	./build/master

