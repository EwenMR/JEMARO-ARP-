from __future__ import print_function

import time
from sr.robot import *

"""
Exercise 3 python script

We start from the solution of the exercise 2
Put the main code after the definition of the functions. The code should make the robot:
	- 1) find and grab the closest silver marker (token)
	- 2) move the marker on the right
	- 3) find and grab the closest golden marker (token)
	- 4) move the marker on the right
	- 5) start again from 1

The method see() of the class Robot returns an object whose attribute info.marker_type may be MARKER_TOKEN_GOLD or MARKER_TOKEN_SILVER,
depending of the type of marker (golden or silver). 
Modify the code of the exercise2 to make the robot:

1- retrieve the distance and the angle of the closest silver marker. If no silver marker is detected, the robot should rotate in order to find a marker.
2- drive the robot towards the marker and grab it
3- move the marker forward and on the right (when done, you can use the method release() of the class Robot in order to release the marker)
4- retrieve the distance and the angle of the closest golden marker. If no golden marker is detected, the robot should rotate in order to find a marker.
5- drive the robot towards the marker and grab it
6- move the marker forward and on the right (when done, you can use the method release() of the class Robot in order to release the marker)
7- start again from 1

	When done, run with:
	$ python run.py exercise3.py

"""

close=1.0
direction=8.0
a_th = 2.0
""" float: Threshold for the control of the orientation"""

d_th = 0.4
""" float: Threshold for the control of the linear distance"""

R = Robot()
""" instance of the class Robot"""


def drive(speed, seconds):
    """
    Function for setting a linear velocity

    Args: speed (int): the speed of the wheels
          seconds (int): the time interval
    """
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0

def turn(speed, seconds):
    """
    Function for setting an angular velocity

    Args: speed (int): the speed of the wheels
          seconds (int): the time interval
    """
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = -speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0


    """
    Function to find the closest token

    Returns:
        dist (float): distance of the closest token (-1 if no token is detected)
        rot_y (float): angle between the robot and the token (-1 if no token is detected)
    """
    dist = 100
    for token in R.see():
        if token.dist < dist:
            dist = token.dist
            rot_y = token.rot_y
    if dist == 100:
        return -1, -1
    else:
        return dist, rot_y

def find_silver_token():
    dist = 100
    for token in R.see():
        if token.info.marker_type=='silver-token':
            if token.dist < dist:
                dist = token.dist
                rot_y = token.rot_y
    if dist == 100:
        return -1, -1
    else:
        return dist, rot_y

def find_gold_token():

    dist = 100
    print(R.see())
    for token in R.see():
        print(token.info.marker_type)
        if token.info.marker_type=='gold-token':
            print("found")
            if token.dist < dist:
                dist = token.dist
                rot_y = token.rot_y
    if dist == 100:
        return -1, -1
    else:
        return dist, rot_y

def silver_token():    
    while(1):
        dist,rot_y=find_silver_token()
        if(dist==-1):
            exit()
        elif(-a_th<=rot_y<=a_th):
            while(dist>=close):
                drive(50,1)
                dist,rot_y=find_silver_token()

            while(dist>d_th):
                drive(10,1)
                dist,rot_y=find_silver_token()
            R.grab()
            turn(50,1)
            R.release()
            break
        elif rot_y<-a_th:
            turn(-2,0.5)
        elif rot_y>a_th:
            turn(2,0.5)


def gold_token():
    while(1):
        dist,rot_y=find_gold_token()
        print(dist)
        if(dist==-1):
            exit()
        elif(-a_th<=rot_y<=a_th):
            while(dist>=close):
                drive(50,1)
                dist,rot_y=find_gold_token()

            while(dist>d_th):
                drive(10,1)
                dist,rot_y=find_gold_token()
            R.grab()
            turn(50,1)
            R.release()
            break
        elif rot_y<-a_th:
            turn(-2,0.5)
        elif rot_y>a_th:
            turn(2,0.5)

def main():
    for i in range (10):
        silver_token()
        print("next")
        gold_token()
    function(exit())








main()