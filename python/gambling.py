# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "Gambler IA"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time, random
import names
import client_api, gui

###############
### GLOBALS ###
###############

TABLE_TURNS = 100

def find_starting(world):
    "Find the starting position of the player"
    size = world.get_size()
    for col in range(size[0]):
        for line in range(size[1]):
            if world.get_cell(col, line)["us"] != 0:
                return (col, line, world.get_cell(col, line)["us"])

###############
### CLASSES ###
###############

class Adventurer():
    "Define an agent"
    def __init__(self, manager, start_point):
        self.m = manager
        self.name = names.get_first_name()
        self.pos = start_point
        self.objective = None
        self.score = 1000 # How much money he have
        self.fixed = False
        self.leader = True

    def communicate(self, bids = []):
        "Return (bool, bid) where bool is if it's a new bid and bid the bid"
        return (False, Bid(self, "human", (2,2), 1))

    def new_turn(self):
        "This is a new turn"
        self.fixed = False

    def accepted(self, objective, position, price):
        "Someone accepted your quest"
        self.objective = objective
        self.position = position
        self.score -= price
        self.leader = True
        self.fixed = True

    def following(self, objective, position, wage):
        "You followed a quest"
        self.objective = objective
        self.position = position
        self.score += wage
        self.fixed = True
        self.leader = False

    def __repr__(self):
        "Return a representation of the adventurer"
        s  = "Adventurer: " + self.name + " (" + str(self.score) + " gills in pocket)\n"
        s += "Current objective: " + str(self.objective)+ " (definite: " + str(self.fixed) + ")\n"
        return s
        

class Bid():
    def __init__(self, bider, action, position, wage):
        "Place a Bid"
        self.bider = bider
        self.action = action
        self.position = position
        self.wage = wage
    def accept(self, acceptor):
        "An acceptor has accepted the bid"
        self.bider.accepted(self.action, self.position, self.wage)
        acceptor.following(self.action, self.position, self.wage)

class GamingHall():
    "Where all adventurers meets and gamble"
    def __init__(self, world):
        "Create a new gaming hall"
        self.adventurers = []
        # Create initial objectives
        self._generate_objectives(world)
        self.objectives = [("human", (1,1), 100)] # List of action, position, wage
        
    def add_adventurer(self, adv):
        "Add an adventurer to the hall"
        self.adventurers.append(adv)

    def del_adventurer(self, adv):
        "He died"
        self.adventurers.remove(adv)

    def new_turn(self, new_world):
        "New turn"
        self._generate_objectives(new_world)
        bids = []
        for i in range(TABLE_TURNS):
            # Find bidders
            bids = [ adv.communicate()[1] for adv in self.adventurers if adv.communicate()[0] == True]
            bids.sort(key=lambda bid : bid.wage, reverse = True) # Sort by best bider
            if len(bids) == 0: # No more bider, end of turn
                break
            # Find acceptors
            acceptors = [ (adv,adv.communicate()[1]) for adv in self.adventurers if adv.communicate()[0] == False]
            if len(bider) == 0: # No more acceptors, end of turn
                break
            random.shuffle(acceptors)
            # Accept bid
            bid = acceptors[0][1]
            bid.accept(acceptors[0][0])
        # End of turn everyone has something to do, print it
        print "###################"
        print "### END OF TURN ###"
        print ""
        for adv in self.adventurers:
            print adv
        print "###################"

    def _generate_objectives(self, world):
        "Generate a list of objectives on the map and store it"
        self.objectives = []
        for x in range(world.get_size()[0]):
            for y in range(world.get_size()[1]):
                cell = world.get_cell(x, y)
                if cell["human"] != 0:
                    self.objectives.append(("human", (x, y), cell["human"]))
                if cell["ennemy"] != 0:
                    self.objectives.append(("ennemy", (x, y), cell["ennemy"]))
            

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
    c = client_api.ClientAPI()
    c.connect("Gambler")
    time.sleep(0.2)
    pos = find_starting(c.get_map())
    world = c.get_map()
    hall = GamingHall(world)
    start = find_starting(world)
    # Create 4 adventurers
    for i in range(4):
        adv = Adventurer(hall, start)
        hall.add_adventurer(adv)
    # Play one turn
    hall.new_turn(world)
    wait = raw_input("...")
    
    
