from __future__ import print_function

import time
from sr.robot import *
NUM_OF_MARKERS = 6

close = 0.7

a_th = 2.0
""" float: Threshold for the control of the orientation"""
d_th = 0.4
""" float: Threshold for the control of the linear distance"""

goal_th_a = 2.0
goal_th_d = 0.9

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
    Function to find the closest token, that has not yet been picked up

    Returns:
        dist (float): distance of the closest token (-1 if no token is detected)
        rot_y (float): angle between the robot and the token (-1 if no token is detected)
    """
    dist = 100
    for token in R.see():
        if token.dist < dist and token.info.code not in goal:
            dist = token.dist
            rot_y = token.rot_y
            code = token.info.code
    if dist == 100:
        return -1,-1,-1
    else:
        return dist, rot_y, code


def find_goal(goal):
    """
    Function to find the location of the gathered tokens

    Returns:
        dist (float): distance of the closest token (-1 if no token is detected)
        rot_y (float): angle between the robot and the token (-1 if no token is detected)
    """
    dist = 100
    for token in R.see():
        # if goal is found
        if token.info.code in goal:
            dist = token.dist
            rot_y = token.rot_y
    if dist == 100:
        print("Can't find the goal\n")
        return -1,-1
    else:
        return dist, rot_y
    

def pick_up(goal):
    """
    Function to pick up the closest token that hasn't been picked up.

    Returns:
        dist (int): code number of the picked up marker

    """
    dist,rot_y,code=find_token(goal)
    turn(rot_y/3,1)

    if(dist==-1):
        exit()

    # Move quickly when it's far away
    while(dist>=close):
        drive(50,1)
        dist,rot_y,code=find_token(goal)

    # Align the robot with the token
    while(-a_th > rot_y or rot_y > a_th):
        turn(10,0.1)
        dist,rot_y,code=find_token(goal)

    # Approach slowly to the token until it is close enough to grab
    while(dist>d_th):
        drive(10,1)
        dist,rot_y,code=find_token(goal)
    R.grab()
    return code


def go_to_goal(goal):
    """
    function that takes the robot to the gathered place
    """
    while(1):
        dist,rot_y=find_goal(goal)
        if dist==-1: # if no markers are detected, the program ends
            print("I don't see the goal!!")
            exit()  
        elif dist <goal_th_d: 
            break
        elif -a_th<= rot_y <= a_th: # if the robot is well aligned with the goal, we go forward
            print("Ah, here we are!.")
            if dist>close:
                drive(50,1)
            else:
                drive(10, 0.5)
        elif rot_y < -a_th: # if the robot is not well aligned with the goal, we move it on the left or on the right
            print("Left a bit...")
            turn(-2, 0.5)
        elif rot_y > a_th:
            print("Right a bit...")
            turn(+2, 0.5)




# here goes the code
def main():
    goal = []
    
    for i in range (NUM_OF_MARKERS):
        code = pick_up(goal)
        # set the picked up token as a gather up point(goal)
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


