# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "GUI"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time, curses

###############
### GLOBALS ###
###############

###############
### CLASSES ###
###############

class GUI(object):
    "Manage a curses GUI"
    def __init__(self, key_callback = lambda a : a):
        "Initialize the screen"
        # See https://docs.python.org/2/howto/curses.html
        self.__callback = key_callback
        self.scr = curses.initscr()
        curses.noecho()
        curses.cbreak()
        self.scr.keypad(1)
        self.scr.nodelay(1)
        curses.curs_set(0)
        curses.start_color()
        curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_GREEN)
        curses.init_pair(2, curses.COLOR_BLACK, curses.COLOR_RED)
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_YELLOW)
        curses.init_pair(4, curses.COLOR_BLACK, curses.COLOR_WHITE)

    def close(self):
        "Return the screen to normal"
        self.scr.clear()
        self.scr.refresh()
        curses.nocbreak()
        #self.scr.keypad(0)
        curses.echo()
        curses.endwin()

    def __enter__(self):
        "RAII Return the screen with the keyword with"
        return self
    
    def __exit__(self, type, value, traceback):
        "Return the console to normal"
        self.close()
    def __draw_borders(self, world):
        "Draw world borders"
        matrix = world.get_world_as_matrix()
        for col in range(len(matrix)):
            for line in range(len(matrix[col])):
                if (col+line) % 2 == 0:
                    self.scr.addstr(col, line, " ", curses.color_pair(4))
                

    def show(self, world):
        "Show a World matrix"
        self.scr.clear()
        self.__draw_borders(world)
        matrix = world.get_world_as_matrix()
        for col in range(len(matrix)):
            for line in range(len(matrix[col])):
                cell = matrix[col][line]
                if cell["us"] != 0:
                    self.scr.addstr(line, col, str(cell["us"]), curses.color_pair(1))
                elif cell["ennemy"] != 0:
                    self.scr.addstr(line, col, str(cell["ennemy"]), curses.color_pair(2))
                elif cell["human"] != 0:
                    self.scr.addstr(line, col, str(cell["human"]), curses.color_pair(3))
                #self.scr.addstr(col, line, "T")
        # Then check for a KeyboardInterrupt since we disabled it
        key = self.scr.getch()
        if key in [3,113]: # CTRL_C or q
            raise KeyboardInterrupt
        if key != -1:
            self.__callback(key)
        self.scr.refresh()

###################
### DEFINITIONS ###
###################

##################
###  __MAIN__  ###
##################

if __name__ == "__main__":
    print "> Welcome to " + str(prog_name) + " (r" + str(version) + ")"
    print "> By Viq (under CC BY-SA 3.0 license)"
    print "> Loading program ..."
    from client_api import World
    w = World(20,5)
    with GUI() as a:
        a.show(w)
        time.sleep(2)
