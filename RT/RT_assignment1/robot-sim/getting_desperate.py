from __future__ import print_function

import time
from sr.robot import *

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

        
def find_token():

    dist = 1000
    rot_y = 0
    code = 0
    print(regroup)
    for token in R.see():
        if token.dist < dist and (token.info.code not in regroup):
            dist = token.dist
            rot_y = token.rot_y
            code = token.info.code
    print(code)
    if dist == -1:
        return -1,-1,-1
    else:
        return dist, rot_y, code

def find_token():

    dist = 1000
    rot_y = 0
    code = 0
    print(regroup)
    for token in R.see():
        if token.dist < dist and (token.info.code not in regroup):
            dist = token.dist
            rot_y = token.rot_y
            code = token.info.code
    print(code)
    if dist == 1000:
        return -1,-1,-1
    else:
        return dist, rot_y, code




        
def find_regroup():

    rdist = 100
    rrot_y = 0

    for token in R.see():
        if token.info.code in regroup:
            rdist = token.dist 
            rrot_y = token.rot_y
    if rdist == 100:
        return -1,-1
    else:
        return rdist, rrot_y



search = True
goal = 1
regroup=[] 
while 1:
    if search is True:

        dist, rot_y, code = find_token()
        # if no token code in regroup choose closes one
        print(f"This is the distance {dist}")

        # if not regroup:
        #     regroup.append(code)

        if dist == -1:
            print("I can't see a thing")
            turn(-20,0.5)
        elif dist <= d_th:
            print("I am able to reach the box")
            # time.sleep(0.5)
            # Here when I grab i append the code to the regroup array and make search false so that the next loop starts
            if R.grab():                
                print("gotcha")
                search = False
                
                if goal == 1:
                    turn(60,1)
                    R.release()
                    print("released")
                    regroup.append(code)
                    goal = 0
                    search = True
                # regroup.append(code)
                print(search)
                
                # failed grab error msg and back up to try again
            else:
                print("my hands are too small")
                drive(-20,1)
        # the robot is in line with the token
        elif -a_th < rot_y < a_th:
            print("straight ahead")
            drive(30,0.5)
        # correcting the angle
        elif rot_y < -a_th:
            print("lefty")
            turn(-2,0.5)
        # correcting the angle
        elif rot_y > a_th:
            print("righty")
            turn(+2,0.5)


    # this is the case when we are searching for the regroupment
    else:
        rdist, rrot_y = find_regroup()

        if rdist == -1:
            print("I'm lost")
            turn(+20,0.5)
        # if we are within the threshold then release the object and search becomes true to look for another token
        elif rdist <= d_th+0.3:
            R.release()
            regroup.append(code)
            search = True
            print(search)
        # robot in line with token
        elif -a_th < rrot_y < a_th:
            drive(50,0.5)
            print("no")
        # adjusting agnle
        elif rrot_y < -a_th:
            print("turn left a little")
            turn(-2,0.5)
        # adjusting angle
        elif rrot_y > a_th:
            print("righty")
            turn(+2,0.5)