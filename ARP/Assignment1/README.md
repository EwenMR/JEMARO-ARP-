# Project Name

## Overview

A brief description of the project and its purpose.

## Architecture Sketch

![Architecture sketch](https://github.com/kohei-tateyama/JEMARO/blob/master/ARP/Assignment1/resources/architecture2.jpg)


# Short definition of all Active Components

There are 3 folders `build`, `include` and `src` as well as a makefile. The build folder holds all the compiled and executbale files that ar made by running the makefile. The `include` directory contains a header file `constants.h`. The `src` directory contains `drone.c`, `keyboardManager.c`, `master.c`, `server.c`, `watchdog.c` and `window.c`. 


## constants.h

The `constants.h` file defines various constants and parameters that are used through the program. 
This includes, shared memory key, size and path, semaphore path, constants about the rows and columns of the window, values for the mass of the drone, the viscosity constant, the integration interval and the length of messages. 

This header file is included in every .c file as to ensure that changes to these values can be easily managed and changed from a centralized location.


## master.c

The `master.c` file is a component of the program responsible for forking the other processes. This allows for the execution of different components that include the server, the window, the keyboard manager, the drone and the watchdog in separate konsole windows. 

At the beginning of the file a `summon()` function is created that serves the purpose of executing other functions using `execvp()` and returning an error if it has failed. This will be used in a for loop that will iterate for the number of processes in the program. Each iteration a new child process is created using `fork()` which then calls for the `summon()` of each initialised argument representing all processes.

Furthermore, the master file creates the file descriptors and pipes for the passing of data in between the processes. Lastly, a `for` loop is created to make the master terminate if any of the children are terminated. 


## watchdog.c

The `watchdog.c` file is resposnible for the monitoring (the 'health') and managaing of the other processes. It checks to see if the processes are running correctly. The program uses 'signals' and 'pipes' to carry out this task. Once it has received the ID of the processes, it sends signals to all processes and waits for a response. If the watchdog receives a signal back from a process, it resets the timer of that process. If the timer were to exceed a defined threshold, a signal would be sent to terminate all processes and exits. In all different files you will find elements that are related to the watchdog for this purpose.


## server.c

The main role `server.c` file is to create and update the shared memory made for the drone's position, as well as the creation of a semaphore that will allow to have synchronized access to this shared memory. Additionally, the file sets up signal handling and sends the server's process ID to the watchdog through pipes.


## keyboardManager.c

The `keyboardManager.c` file is made to manage the user input from the keyboard and communicate the corresponding commands to the `drone.c` file to control the drone's movement. Different keys have different associated movements. The code receives the user input from the window, interprets the commands and sends commands using pipes. Signal handling is also implemented to send a signal to the watchdog.


## drone.c

The `drone.c` files as briefly alluded to in the previous part is made to control the movement and updating the position of the drone based off of the control commands received from the keyboarManager though pipes. It calculates the position using Euler's method of a given equation of motion. The file uses shared memory and semaphores to send the updated position to the window file. This file contains signal handling to be able to send a signal to the watchdog.


## window.c

The `window.c` creates the graphical user interface of the game. Using the ncurses library, a main window where the drone can be controlled and a smaller window with the position of the drone are made. The file communicates with the keyboardManager using pipes and uses shared memory to read the updates position of the drone. Once again signal handling is taken care of as to send a signal to the watchdog.



## Instruction for installing and running
These instructions will help you get a copy of the project up and running on your local machine.

### Prerequisites

Make sure you have the following installed:

- [Make](https://www.gnu.org/software/make/)

### Clone the Repository

```bash
$ git clone https://github.com/kohei-tateyama/JEMARO.git
$ cd JEMARO/ARP/Assignment1 
```

### Build the Project

```bash
$ make
```
This command will compile all the necessary files.

### Execute the Project

```bash
$ make run
```
This command will run the master file which will fork all the child processes.


## Operational instructions

## Anything else????
