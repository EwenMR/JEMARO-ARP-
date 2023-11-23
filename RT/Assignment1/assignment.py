from __future__ import print_function

import time
from sr.robot import *
NUM_OF_MARKERS = 6


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
    dist=-1
    while(1):
        dist,rot_y,code=find_token(goal)

        if (dist == -1):
            turn(100,0.1)
        elif(dist <= d_th):
            R.grab()
            break
        # Align the robot with the token
        elif -a_th<= rot_y <= a_th: # if the robot is well aligned with the goal, we go forward
            print("Ah, here we are!.")
            if dist> d_th + 0.3:
                drive(100,0.1)
            else:
                drive(10, 0.5)
        elif rot_y < -a_th: # if the robot is not well aligned with the goal, we move it on the left or on the right
            print("Left a bit...")
            turn(-2, 0.5)
        elif rot_y > a_th:
            print("Right a bit...")
            turn(+2, 0.5)
        # Approach slowly to the token until it is close enough to grab
        elif (dist<d_th+0.3):
            drive(10,0.1)
        # Move quickly when it's far away
        elif (dist>=d_th+0.3):
            drive(50,1)

    return code


def go_to_goal(goal):
    """
    function that takes the robot to the gathered place
    """
    while(1):
        dist,rot_y=find_goal(goal)
        if dist==-1: # if no markers are detected, the program ends
            print("I don't see the goal!!")
            turn(20,1)
        elif dist <= d_th + 0.3: 
            break
        elif -a_th<= rot_y <= a_th: # if the robot is well aligned with the goal, we go forward
            print("Ah, here we are!.")
            if dist> d_th + 0.3:
                drive(100,0.1)
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
    i = 0
    
    while(1):
        if len(goal) == NUM_OF_MARKERS:
            print("FINISHED")
            break
        code = pick_up(goal)
        print("grab")
        # set the picked up tokens as a gather up point(goal)
        

        if i==0:
            turn(60,1)
            i=1
            print("first")
        else:
            print("go to goal")
            go_to_goal(goal)
        
        print("release")
        R.release()
        goal.append(code)
        print(goal)
        drive(-30,1)



main()


