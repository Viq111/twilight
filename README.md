Pylight - An OK IA
===================

Let you win the Vampires VS Wolves game

## Prerequisites

- [Python 2.7](https://www.python.org/downloads/)
- (Optional) [Nuitka](http://nuitka.net/pages/download.html) -> To compile python code to C++ and have a faster code
- [Python Pip](https://pypi.python.org/pypi/pip) (See below for installation)

## Installing Pip (if you already have it, skip this section)

To manage dependencies, you will need [pip](https://pypi.python.org/pypi/pip). The whole installation procedure is available [here](https://pip.pypa.io/en/latest/installing.html)
but in most cases you an simply do:

- [Download pip](https://bootstrap.pypa.io/get-pip.py) or `wget https://bootstrap.pypa.io/get-pip.py`
- Open a terminal and install it: `python get-pip.py`
- Close and reopen you terminal and check installation with `pip --version`
- Enjoy

## Installing dependancies

One you have python and pip, lone this repository and do:

```bash
cd python
pip install -r requirements.txt
```

## Running the AI

Fist test the AI with: `python launcher.py` and press return (enter) twice to quit the server and both AIs

You will need a game server which is available in the server/official directory.
You can then launch each AI with:

```bash
python ai_gambling.py
python ai_nearest_possible.py
python ai_objective_path.py
```

## Python folder structure

### client_api.py

client_api stores the internals to communicate with the server. ClientAPI should be inited and then called with the AI to instanciate each reset.
World object provide easy functions (such as A*) to let you move through the world.

### gui.py

Helper lib to show a board. Should not be used alone, use manual_input instead.

### manual_input.py

Let you play as a human by moving your whole units each turn

### minmax.py

Library to implement MiniMax (with Alpha Beta) used to find the best moves.

### ai_nearest_possible.py

Nearest Possible is the easier IA to understand, it tries to find the nearest prey and go to it

### ai_objective_path.py

Objective Path is an AI that still go to objectives one by one but use the MiniMax algorithm to select the best one (not only the nearest one)

### ai_gambling.py

Gambling is an outdated AI which was supposed to play as a multi-agent system.

### ai_pylight.py

Pylight is our main AI, it derivates from ObjectivePath but once it has an objective, it tries to free a group with the remaining units and let it live its life.
Once the satellite has done its work, it will return to the parent. Hence this IA is a really fast splitting AI.