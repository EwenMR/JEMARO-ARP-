
## Installing and running
To run this code, python 3 must be installed, as well as some libraries (pypybox2d, pygame, time, threading). Once all the libraries are installed, clone this github repository using the command below.
```bash
$ git clone https://github.com/kohei-tateyama/JEMARO/tree/master/RT/Assignment1/robot-sim.git
```
Now to run the code, in the robot-sim directory, 

```bash
$ python3 run.py assignment.py
```
This should successfully open a new window for the simulator, and the simulator should collect all the boxes to the center.

![Result](https://github.com/kohei-tateyama/JEMARO/tree/master/RT/Assignment1/robot-sim/Result.png)

## Functions

### `drive(speed, seconds)`

- Start and stop the motors for the specified duration.
- Args:
  - `speed (int)`: Speed of the wheels.
  - `seconds (int)`: Time interval.



```python
def drive(speed, seconds):
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0

```



### `turn(speed, seconds)`

- Start and stop the motors for the specified duration while turning the robot.
- Args:
  - `speed (int)`: Speed of the wheels.
  - `seconds (int)`: Time interval.




```python
def turn(speed, seconds):
    R.motors[0].m0.power = speed
    R.motors[0].m1.power = -speed
    time.sleep(seconds)
    R.motors[0].m0.power = 0
    R.motors[0].m1.power = 0

```


### `find_token(goal)`

- Find the closest token that hasn't been picked up.
- Returns:
  - `dist (float)`: Distance of the closest token (-1 if none detected).
  - `rot_y (float)`: Angle between the robot and the token (-1 if none detected).
  - `goal (int)`: The 'code' of the closest token will be returned.

```python
def find_token(goal):
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

```


### `find_goal(goal)`

- Find the location of the gathered tokens(goal location).
- Arg:
  - `goal (list of ints)`: The 'code' of tokens that have been picked up and gathered together, will be stored in this list. In this function, goal will be used to find the token that is gathered together, to set that token as a goal.
- Returns:
  - `dist (float)`: Distance of the closest token (-1 if none detected).
  - `rot_y (float)`: Angle between the robot and the token (-1 if none detected).


```python
def find_goal(goal):
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

```


### `pick_up(goal)`

- Pick up the closest token that hasn't been picked up already.
- Args:
  - `goal (list of ints)`: The 'code' of tokens that have been picked up and gathered together, will be stored in this list. In this function, goal will be used to prevent picking the same token once it is already gathered up.
- Returns:
  - `code (int)`: Code number of the picked-up token.


```python
def pick_up(goal):
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


```

### `go_to_goal(goal)`

- Move the robot to the gathered place.
- Args: 
  - `goal (list of ints)`: The 'code' of tokens that have been picked up and gathered together, will be stored in this list. In this function, goal will be used as an argument to call the 'find_goal' function.


```python
def go_to_goal(goal):
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

```



### Main Program

- Initialize a list to keep track of gathered markers.
- Loop to pick up markers:
  - Call `pick_up` to pick up the closest token.
  - Set the picked-up token as a gather-up point (goal).
  - Adjust robot's orientation and movement.
- Release the markers and continue the process.




-----------------------------

Robot API
---------

The API for controlling a simulated robot is designed to be as similar as possible to the [SR API][sr-api].

### Motors ###

The simulated robot has two motors configured for skid steering, connected to a two-output [Motor Board](https://studentrobotics.org/docs/kit/motor_board). The left motor is connected to output `0` and the right motor to output `1`.

The Motor Board API is identical to [that of the SR API](https://studentrobotics.org/docs/programming/sr/motors/), except that motor boards cannot be addressed by serial number. So, to turn on the spot at one quarter of full power, one might write the following:

```python
R.motors[0].m0.power = 25
R.motors[0].m1.power = -25
```

### The Grabber ###

The robot is equipped with a grabber, capable of picking up a token which is in front of the robot and within 0.4 metres of the robot's centre. To pick up a token, call the `R.grab` method:

```python
success = R.grab()
```

The `R.grab` function returns `True` if a token was successfully picked up, or `False` otherwise. If the robot is already holding a token, it will throw an `AlreadyHoldingSomethingException`.

To drop the token, call the `R.release` method.

Cable-tie flails are not implemented.

### Vision ###

To help the robot find tokens and navigate, each token has markers stuck to it, as does each wall. The `R.see` method returns a list of all the markers the robot can see, as `Marker` objects. The robot can only see markers which it is facing towards.

Each `Marker` object has the following attributes:

* `info`: a `MarkerInfo` object describing the marker itself. Has the following attributes:
  * `code`: the numeric code of the marker.
  * `marker_type`: the type of object the marker is attached to (either `MARKER_TOKEN_GOLD`, `MARKER_TOKEN_SILVER` or `MARKER_ARENA`).
  * `offset`: offset of the numeric code of the marker from the lowest numbered marker of its type. For example, token number 3 has the code 43, but offset 3.
  * `size`: the size that the marker would be in the real game, for compatibility with the SR API.
* `centre`: the location of the marker in polar coordinates, as a `PolarCoord` object. Has the following attributes:
  * `length`: the distance from the centre of the robot to the object (in metres).
  * `rot_y`: rotation about the Y axis in degrees.
* `dist`: an alias for `centre.length`
* `res`: the value of the `res` parameter of `R.see`, for compatibility with the SR API.
* `rot_y`: an alias for `centre.rot_y`
* `timestamp`: the time at which the marker was seen (when `R.see` was called).

For example, the following code lists all of the markers the robot can see:

```python
markers = R.see()
print "I can see", len(markers), "markers:"

for m in markers:
    if m.info.marker_type in (MARKER_TOKEN_GOLD, MARKER_TOKEN_SILVER):
        print " - Token {0} is {1} metres away".format( m.info.offset, m.dist )
    elif m.info.marker_type == MARKER_ARENA:
        print " - Arena marker {0} is {1} metres away".format( m.info.offset, m.dist )
```

[sr-api]: https://studentrobotics.org/docs/programming/sr/
