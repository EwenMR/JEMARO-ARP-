# Assignment 3
Team Name: WhereIsTheMarket?

Team Members: Ewen Gay-Semenkoff, Kohei Tateyama

## Architecture Sketch

![Architecture sketch](https://github.com/kohei-tateyama/JEMARO/blob/master/ARP/Assignment3/resources/Assignment3archi3.png)


# Short definition of all Active Components

There are 5 folders `build`, `include`, `src`, `log` and `resources`  as well as a makefile. The build folder holds all the compiled and executbale files that are made by running the makefile. The `include` directory contains header files `constants.h`, `log.c`, `utility.c` . The `src` directory contains `drone.c`, `keyboardManager.c`, `master.c`, `server.c`, `watchdog.c`, `window.c`, `obstacle.c`, and `target.c`. The `log` directory contains the log files for debugging and observability.


## constants.h

The `constants.h` file defines various constants and parameters that are used through the program. 
This includes constants about the rows and columns of the window, number of obstacles and targets, values for the mass of the drone, the viscosity constant, the integration interval, the length of messages and more. 

This header file is included in every .c file as to ensure that changes to these values can be easily managed and changed from a centralized location.


## master.c

The `master.c` file is a component of the program responsible for forking the other processes. This allows for the execution of different components that include the server, the window, the keyboard manager, the drone and the watchdog in separate konsole windows. 

At the beginning of the file a `summon()` function is created that serves the purpose of executing other functions using `execvp()` and returning an error if it has failed. This will be used in a for loop that will iterate for the number of processes in the program. Each iteration a new child process is created using `fork()` which then calls for the `summon()` of each initialised argument representing all processes.

Furthermore, the master file creates the file descriptors and pipes for the passing of data in between the processes. Lastly, a `for` loop is created to make the master terminate if any of the children are terminated. 


## watchdog.c

The `watchdog.c` file is resposnible for the monitoring (the 'health') and managaing of the other processes apart from the obstacles and targets in this assignment. It checks to see if the processes are running correctly. The program uses 'signals' and 'pipes' to carry out this task. Once it has received the ID of the processes, it sends signals to all processes and waits for a response. If the watchdog receives a signal back from a process, it resets the timer of that process. If the timer were to exceed a defined threshold, a signal would be sent to terminate all processes and exits. In all different files you will find elements that are related to the watchdog for this purpose.


## server.c

The main role `server.c` file is to serve as the central coordinator in a multi-process communication system. Pipes are used to receive and store data (like drone position) in local variables as well as send the data to the necessary processes. Additionally, in this assignment, the server acts as socket server, which can connect to multiple clients and receive as well as send data. Specifically, in our case the socket server will have the role of connecting to the target and obstacle socket clients from another group. Furthermore, the file sets up signal handling and sends the server's process ID to the watchdog through pipes. 



## keyboardManager.c

The `keyboardManager.c` file is made to manage the user input from the keyboard and communicate the corresponding commands to the `drone.c` file to control the drone's movement. Different keys have different associated impacts on the command force which is used to calculate new forces. The code receives the user input from the window, interprets the commands and sends commands using pipes. Signal handling is also implemented to send a signal to the watchdog.


## drone.c

The `drone.c` files as briefly alluded to in the previous part is made to control the movement and updating the position of the drone based off of the control commands received from the keyboarManager though pipes. It calculates the position using Euler's method of a given equation of motion. Additionally, it calculates the repulsive forces associated with the obstacles and the walls of the game. The file uses pipes to send the updated position to the window file. This file contains signal handling to be able to send a signal to the watchdog.


## window.c

The `window.c` creates the graphical user interface of the game. Using the ncurses library, a main game window with the drone, the obstacles and the targets was made above a smaller window with the position of the drone.
The file receives the drone, target and obstalce positions and sends the updated data through pipes. Once again signal handling is taken care of as to send a signal to the watchdog.


## obstacle.c
The role of the `obstacle.c` file is to create and manage the random spawning of the obstacle positions. The file reads the drone position as to not create an obstacle too close to the drone to avoid any unwanted behaviour. The obstacles file is made into a socket client that sends its positions via sockets to an external server from another gorup. 


## target.c
The `target.c` file creates the random target positions as long as they are not within a certain threshold of the drone's position. Like the obstacles file the targets file is a socket client that sends its positions to an external socket server using sockets.



## Instruction for installing and running
These instructions will help you get a copy of the project up and running on your local machine.


### Clone the Repository
To clone this project to the local file, enter:

```bash
$ git clone https://github.com/kohei-tateyama/JEMARO.git
$ git checkout master
$ cd JEMARO/ARP/Assignment3
```
or unzip the file that is submitted to the aulaweb page.

### Build and Execute the Project
In the 'Assignment3' directory, hit:

```bash
$ make
```
to compile all the necessary files. And to run the project as a server, enter:

```bash
$ ./build/master "server" 8080 
```
to compile all the necessary files. "8080" can be any available port number as long as you use the same number on both server and client. And to run the project as clients (generating targets and obstacles), enter:

```bash
$ ./build/master "client" 8080 ip_address
```

replace the ip_address with the actual ip address of the server side.

To run it locally, run
```bash
$ make runServer
```
```bash
$ make runClient
```

## Operational instructions
In the 'window' konsole, there should be a drone displayed as 'X'. Use the keys 

`w` `e` `r` &nbsp; &nbsp;    `u` `i` `o`     

`s` `d` `f`  or              `j` `k` `l`     

`x` `c` `v` &nbsp; &nbsp;    `m` `,` `.`     

to add velocity to the drone. Press it multiple times to make it move faster. To stop the drone immediately, press the keys `d` or `k`. And to terminate all processes, press `space` in the 'window', or close any of the processes.


## What to expect
The general behaviour between the socket server and socket clients (targets and obstacles) is described below.

The following describes, the nature of the communication between the target client and the socket server:
![Targets example](https://github.com/kohei-tateyama/JEMARO/blob/master/ARP/Assignment3/resources/TargServ.png)

The following describes the nature of the communication between the obstacles client and the socket server:
![Architecture sketch](https://github.com/kohei-tateyama/JEMARO/blob/master/ARP/Assignment3/resources/ObServ.png)

Some recurring features in both images:
- "TI" and "OI" are used as identifiers, so that the server can identify each client.
- After the identifiers are received the server sends the window size to the clients so that they may produce correctly scaled target and obstacle positions
- The positions are sent with an identifier followed by a number in a sqaure bracket stating the amount of positions being sent, the positions have the following format: 'x,y' separated by '|'.
- The message "STOP" is sent from server to clients to announce the termination of the game. After receiving this message the clients will die.
- Specifically for the targets client, once all targets have been reached the server sends a Game End message "GE" to make the targets client generate some new target positions.
- Finally, the messages passed are repeated because an echo from the receiver of the message going either way is mandatory.
