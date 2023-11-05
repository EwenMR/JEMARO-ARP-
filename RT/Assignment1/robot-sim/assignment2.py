from __future__ import print_function

import time
from sr.robot import *

close = 0.7
goal = 0.5

a_th = 2.0
""" float: Threshold for the control of the orientation"""
d_th = 0.4
""" float: Threshold for the control of the linear distance"""

goal_th_a = 3.0
goal_th_d = 1.0

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


def find_token(goal):
    """
    Function to find the closest token

    Returns:
        dist (float): distance of the closest token (-1 if no token is detected)
        rot_y (float): angle between the robot and the token (-1 if no token is detected)
    """
    dist = 100
    for token in R.see():
        if token.dist < dist and token.info.code != goal:
            dist = token.dist
            rot_y = token.rot_y
            code = token.info.code
    if dist == 100:
        return -1, -1, -1
    else:
        return dist, rot_y, code

def num_token():
    num=0
    for token in R.see():
        num+=1
    return num


def find_goal(goal):
    dist = 100
    for token in R.see():
        print(token)
        if token.info.code in goal:
            print("found goal")
            dist = token.dist
            rot_y = token.rot_y
    if dist == 100:
        return -1, -1
    else:
        return dist, rot_y
    
def go_to_goal(goal):
    dist,rot_y=find_goal(goal)
    turn(rot_y/3,1)

    if(dist==-1):
        exit()

    while(dist>=close):
        drive(50,1)
        dist,rot_y=find_goal(goal)

    while(-goal_th_a > rot_y or rot_y > goal_th_a):
        turn(10,0.1)
        dist,rot_y=find_goal(goal)

    while(dist>goal_th_d):
        drive(10,1)
        dist,rot_y=find_goal(goal)

# here goes the code
def main():
    goal = []
    for i in range (0,6):
        dist,rot_y,code=find_token(goal)
        turn(rot_y/3,1)

        if(dist==-1):
            exit()

        while(dist>=close):
            drive(50,1)
            dist,rot_y,code=find_token(goal)

        while(-a_th > rot_y or rot_y > a_th):
            turn(10,0.1)
            dist,rot_y,code=find_token(goal)

        while(dist>d_th):
            drive(10,1)
            dist,rot_y,code=find_token(goal)
        R.grab()
        goal.append(code)
        if i==0:
            turn(-15,1)
            drive(100,2)
        else:
            turn(60,1)
            go_to_goal(goal)
        
        R.release()
        drive(-15,1)
        turn(30,1)
        drive(50,1)



main()


