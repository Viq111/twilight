Twilight - An OK IA
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
python gambling.py
python ai_nearest_possible.py
python ai_objective_path.py
```

**Press return to exit the AI when the game is over, it cannot autodetect the end!**
