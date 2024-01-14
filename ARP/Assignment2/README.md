# Assignment 2
Team Name: WhereIsTheMarket?

Team Members: Ewen Gay-Semenkoff, Kohei Tateyama

## Architecture Sketch

![Architecture sketch](https://github.com/kohei-tateyama/JEMARO/blob/master/ARP/Assignment2/resources/ARP2_flowchart2.jpg)


# Short definition of all Active Components

There are 5 folders `build`, `include`, `src`, `log` and `resources`  as well as a makefile. The build folder holds all the compiled and executbale files that are made by running the makefile. The `include` directory contains header files `constants.h`, `log.c`, `utility.c` . The `src` directory contains `drone.c`, `keyboardManager.c`, `master.c`, `server.c`, `watchdog.c` and `window.c`. The `log` directory contains the log files for debugging and observability.


## constants.h

The `constants.h` file defines various constants and parameters that are used through the program. 
This includes constants about the rows and columns of the window, number of obstacles and targets, values for the mass of the drone, the viscosity constant, the integration interval, the length of messages and more. 

This header file is included in every .c file as to ensure that changes to these values can be easily managed and changed from a centralized location.


## master.c

The `master.c` file is a component of the program responsible for forking the other processes. This allows for the execution of different components that include the server, the window, the keyboard manager, the drone and the watchdog in separate konsole windows. 

At the beginning of the file a `summon()` function is created that serves the purpose of executing other functions using `execvp()` and returning an error if it has failed. This will be used in a for loop that will iterate for the number of processes in the program. Each iteration a new child process is created using `fork()` which then calls for the `summon()` of each initialised argument representing all processes.

Furthermore, the master file creates the file descriptors and pipes for the passing of data in between the processes. Lastly, a `for` loop is created to make the master terminate if any of the children are terminated. 


## watchdog.c

The `watchdog.c` file is resposnible for the monitoring (the 'health') and managaing of the other processes. It checks to see if the processes are running correctly. The program uses 'signals' and 'pipes' to carry out this task. Once it has received the ID of the processes, it sends signals to all processes and waits for a response. If the watchdog receives a signal back from a process, it resets the timer of that process. If the timer were to exceed a defined threshold, a signal would be sent to terminate all processes and exits. In all different files you will find elements that are related to the watchdog for this purpose.


## server.c

The main role `server.c` file is to serve as the central coordinator in a multi-process communication system. Pipes are used to receive and store data (like drone position, obstacle positions, and target positions) in local variables as well as send the data to the necessary processes. Additionally, the file sets up signal handling and sends the server's process ID to the watchdog through pipes. 



## keyboardManager.c

The `keyboardManager.c` file is made to manage the user input from the keyboard and communicate the corresponding commands to the `drone.c` file to control the drone's movement. Different keys have different associated movements. The code receives the user input from the window, interprets the commands and sends commands using pipes. Signal handling is also implemented to send a signal to the watchdog.


## drone.c

The `drone.c` files as briefly alluded to in the previous part is made to control the movement and updating the position of the drone based off of the control commands received from the keyboarManager though pipes. It calculates the position using Euler's method of a given equation of motion. Additionally, it calculates the repulsive forces associated with the obstacles and the walls of the game. The file uses pipes to send the updated position to the window file. This file contains signal handling to be able to send a signal to the watchdog.


## window.c

The `window.c` creates the graphical user interface of the game. Using the ncurses library, a main game window with the drone, the obstacles and the targets was made above a smaller window with the position of the drone.
The file receives and sends the appropriate data throigh pipes. Once again signal handling is taken care of as to send a signal to the watchdog.

<<<<<<< HEAD
# target.c
The `target.c` randomly creates 10 targets.
=======

## obstacle.c
The role of the `obstacle.c` file is to create and manage the random spawning of the obstacle positions. The file reads the drone position as to not create an obstacle too close to the drone to avoid any unwanted behaviour. The obstacles' positions are sent via pipes. The signal handling is done as to send a signal to the watchdog as the previous processes.


## target.c
The `target.c` file creates the random target positions as long as they are not within a certain threshold of the drone's position. It also checks whether the target has been reached by the drone. Along the other processes the signal handling is done such that the signal is sent to the watchdog.

>>>>>>> bc3c09b0f697e6e393a467d5fe27f1e24463198f

## Instruction for installing and running
These instructions will help you get a copy of the project up and running on your local machine.


### Clone the Repository
To clone this project to the local file, enter:

```bash
$ git clone https://github.com/kohei-tateyama/JEMARO.git
$ cd JEMARO/ARP/Assignment2
```
or unzip the file that is submitted to the aulaweb page.

### Build and Execute the Project
In the 'Assignment1' directory, hit:

```bash
$ make
```
to compile all the necessary files. And to run the project, enter:

```bash
$ make run
```
This command will open a konsole window. And  `drone`, `keyboard`, `server`, `window`, and `watchdog`.

## Operational instructions
In the 'window' konsole, there should be a drone displayed as 'X'. Use the keys 

`w` `e` `r` &nbsp; &nbsp;    `u` `i` `o`     

`s` `d` `f`  or              `j` `k` `l`     

`x` `c` `v` &nbsp; &nbsp;    `m` `,` `.`     

to add velocity to the drone. Press it multiple times to make it move faster. To stop the drone immediately, press the keys `d` or `k`. And to terminate all processes, press `space` in the 'window', or close any of the processes.
