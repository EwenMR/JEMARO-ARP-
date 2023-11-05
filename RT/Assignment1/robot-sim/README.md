## Pseudocode


### Initialization

- Import necessary libraries.
- Initialize the robot and set various thresholds.

### Movement Functions

#### `drive(speed, seconds)`

- Set linear velocity for the robot's wheels.
- Args:
  - `speed (int)`: Speed of the wheels.
  - `seconds (int)`: Time interval.
- Start and stop the motors for the specified duration.

#### `turn(speed, seconds)`

- Set angular velocity for the robot's wheels.
- Args:
  - `speed (int)`: Speed of the wheels.
  - `seconds (int)`: Time interval.
- Start and stop the motors for the specified duration while turning the robot.

### Marker Detection

#### `find_token(goal)`

- Find the closest token that hasn't been picked up.
- Returns:
  - `dist (float)`: Distance of the closest token (-1 if none detected).
  - `rot_y (float)`: Angle between the robot and the token (-1 if none detected).

#### `find_goal(goal)`

- Find the location of the gathered tokens.
- Returns:
  - `dist (float)`: Distance of the closest token (-1 if none detected).
  - `rot_y (float)`: Angle between the robot and the token (-1 if none detected).

### Marker Handling

#### `pick_up(goal)`

- Pick up the closest token that hasn't been picked up.
- Returns:
  - `dist (int)`: Code number of the picked-up marker.

#### `go_to_goal(goal)`

- Move the robot to the gathered place.

### Main Program

- Initialize a list to keep track of gathered markers.
- Loop to pick up markers:
  - Call `pick_up` to pick up the closest token.
  - Set the picked-up token as a gather-up point (goal).
  - Adjust robot's orientation and movement.
- Release the markers and continue the process.

## Usage

- You can use this pseudo code as a reference for implementing marker gathering behavior using the SR API.
- For the actual code implementation, please refer to the provided Python script.

## License

- Specify the license under which the original code is distributed.



## Exercise
-----------------------------

To run one or more scripts in the simulator, use `run.py`, passing it the file names. 

I am proposing you three exercises, with an increasing level of difficulty.
The instruction for the three exercises can be found inside the .py files (exercise1.py, exercise2.py, exercise3.py).

When done, you can run the program with:

```bash
$ python run.py exercise1.py
```

You have also the solutions of the exercises (folder solutions)

```bash
$ python run.py solutions/exercise1_solution.py
```