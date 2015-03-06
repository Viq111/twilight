# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "Manual AI"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time
import client_api
import gui

###############
### GLOBALS ###
###############

###############
### CLASSES ###
###############

class ManualInput():
    "Manage a Manual Input instance"
    def __init__(self, name):
        "Create a Manual Input"
        self.name = name

    def __callback(self, key):
        "Callback when a key is typed"
        new_pos = self.pos
        if key == 258: # Down
            new_pos = (self.pos[0], self.pos[1] + 1)
        elif key == 259: # Up
            new_pos = (self.pos[0], self.pos[1] - 1)
        elif key == 260: # Left
            new_pos = (self.pos[0] - 1, self.pos[1])
        elif key == 261: # Right
            new_pos = (self.pos[0] + 1, self.pos[1])
            
        if new_pos != self.pos:
            nb = find_starting(self.c.get_map())[2]
            # Send command
            self.c.move([(self.pos[0], self.pos[1], nb, new_pos[0], new_pos[1])])
            self.pos = (new_pos[0], new_pos[1], nb)
        
    def mainloop(self):
        "Enter the main loop"
        with client_api.ClientAPI() as self.c, gui.GUI(self.__callback) as self.g:
            self.c.connect(self.name)
            # Get my position
            time.sleep(0.2)
            self.pos = find_starting(self.c.get_map())
            try:
                while 1:
                    self.w = self.c.get_map()
                    self.g.show(self.w)
                    time.sleep(0.2)
            except KeyboardInterrupt:
                pass

###################
### DEFINITIONS ###
###################

def find_starting(world):
    "Find the starting position of the player"
    size = world.get_size()
    for col in range(size[0]):
        for line in range(size[1]):
            if world.get_cell(col, line)["us"] != 0:
                return (col, line, world.get_cell(col, line)["us"])

##################
###  __MAIN__  ###
##################

if __name__ == "__main__":
    print "> Welcome to " + str(prog_name) + " (r" + str(version) + ")"
    print "> By Viq (under CC BY-SA 3.0 license)"
    print "> Loading program ..."
    name = raw_input("Enter your name:")
    manual = ManualInput(name)
    manual.mainloop()
        
