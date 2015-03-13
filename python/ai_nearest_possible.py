# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "AI - Nearest Possible"
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

###############
### GLOBALS ###
###############

###############
### CLASSES ###
###############

class Nearest_possible():
    "A simple AI which take the best nearest objective"
    def __init__(self, client):
        "Connect to the client"
        client.set_callback(self.callback)
        client.connect("Near")
        self.c = client
        self.pos = self.c.get_map().get_starting_position()
        self.number = self.c.get_map().get_cell(self.pos[0], self.pos[1])["us"]

    def callback(self, world):
        "Play nearest objective"
        # 0 - Update our number and position
        self.pos = world.get_starting_position()
        self.number = world.get_cell(self.pos[0], self.pos[1])["us"]
        
        # 1 - Find objectives
        objectives = []
        for x in range(world.get_size()[0]):
            for y in range(world.get_size()[1]):
                if world.get_cell(x, y)["human"] != 0:
                    objectives.append((x, y, world.get_cell(x, y)["human"]))
        # 2 - Filter only possible objectives
        objectives = filter(lambda obj : obj[2] <= self.number, objectives)
        
        if len(objectives) > 0: # There is still humans on the map that we can take
            # 3 - Compute distance to each objective
            distance = [ (obj[0], obj[1], world.find_path_time(self.pos, (obj[0], obj[1]))) for obj in objectives]
            # 4 - Find nearest objective
            distance.sort(key = lambda d : d[2])
            # 5 - Go there
            goal = world.find_path(self.pos, (distance[0][0], distance[0][1]))
            print("[AI](" + str(self.number) + ") Going for humans at " + str((distance[0][0], distance[0][1])) + " through " + str(goal))
            self.c.move([(self.pos[0], self.pos[1], self.number, goal[0], goal[1])])
        else: # No more humans, attack the other player
            objectives = []
            for x in range(world.get_size()[0]):
                for y in range(world.get_size()[1]):
                    if world.get_cell(x, y)["ennemy"] != 0:
                        objectives.append((x, y, world.get_cell(x, y)["ennemy"]))
            # Find best pray
            objectives.sort(key = lambda e : e[2])
            goal = world.find_path(self.pos, (objectives[0][0], objectives[0][1]))
            print("[AI](" + str(self.number) + ") Going for ennemy at " + str(objectives[0]) + " through " + str(goal))
            self.c.move([(self.pos[0], self.pos[1], self.number, goal[0], goal[1])])
        
        


###################
### DEFINITIONS ###
###################

def main(client):
    "Create an AI and play"
    ai = Nearest_possible(client)
    wait = raw_input("...")

##################
###  __MAIN__  ###
##################

if __name__ == "__main__":
    print "> Welcome to " + str(prog_name) + " (r" + str(version) + ")"
    print "> By Viq (under CC BY-SA 3.0 license)"
    print "> Loading program ..."
    with client_api.ClientAPI() as client:
        main(client)
