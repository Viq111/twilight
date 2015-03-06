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
import os, time, random, math, sys
import names
import client_api

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

    def select_random_objective(self, objectives):
        "Select a new random objective"
        self.fixed = False
        self.leader = True
        self.objective = random.choice(objectives)

    def communicate(self, bids = []):
        "Return (bool, bid) where bool is if it's a new bid and bid the bid"
        # 0 : Do nothing
        # 1 : Accept quest
        # 2 : Propose quest
        if self.objective.obj == "human": # Kill human, this is good bid!
            return (2, Bid(self, self.objective, 20))
        else:
            if len(bids) > 0:
                # Follow something
                return (1, random.choice(bids))
            else:
                # Do nothing
                return (0, None)

    def accepted(self, objective, price):
        "Someone accepted your quest"
        self.objective = objective
        self.score -= price
        self.leader = True
        self.fixed = True

    def following(self, objective, wage):
        "You followed a quest"
        self.objective = objective
        self.score += wage
        self.fixed = True
        self.leader = False

    def __repr__(self):
        "Return a representation of the adventurer"
        s  = "Adventurer: " + self.name + " (" + str(self.score) + " gills in pocket)\n"
        s += "Current objective: " + str(self.objective)+ " (definite: " + str(self.fixed) + ")\n"
        return s
        
class Objective():
    def __init__(self, type, position, number):
        "Define an objective"
        self.obj = type # Type of objetive: "human", "ennemy"
        self.pos = position # Position of the objective
        self.number = number
        if self.obj == "human":
            # Reward tier a la kickstarter
            self.tier = self._create_human_reward(self.number)
        elif self.obj == "ennemy":
            self.tier = self._create_ennemy_reward(self.number)
        else:
            raise RuntimeError("Cannot find objective: " + str(self.obj))
            
    def _create_human_reward(self, number):
        "Return a reward tier"
        # When me < number, P = me/(me+number)
        tier = {number : 100 * number}
        for i in range(1, number):
            reward = 100 * number * i / (i+number)
            tier[i] = reward
        return tier
    def _create_ennemy_reward(self, number):
        "Return a reward tier for the ennemy"
        critic = int(math.ceil(number*1.5))
        tier = {critic : 150 * number}
        for i in range(1, critic):
            reward = 150 * number * i / (i+number)
            tier[i] = reward
        return tier

    def __repr__(self):
        "Show an objetive"
        s = "destroy " + str(self.obj) + " (" + str(self.number) + ") at position " + str(self.pos)
        return s

class Bid():
    def __init__(self, bider, objective, wage):
        "Place a Bid"
        self.bider = bider
        self.objective = objective
        self.wage = wage
    def accept(self, acceptor):
        "An acceptor has accepted the bid"
        self.bider.accepted(self.objective, self.wage)
        acceptor.following(self.objective, self.wage)

class GamingHall():
    "Where all adventurers meets and gamble"
    def __init__(self, world, api):
        "Create a new gaming hall"
        self.adventurers = []
        self.api = api
        # Create initial objectives
        self._generate_objectives(world)
        self.objectives = [("human", (1,1), 100)] # List of action, position, wage
        self.api.set_callback(self.turn_callback)
        
    def add_adventurer(self, adv):
        "Add an adventurer to the hall"
        self.adventurers.append(adv)

    def del_adventurer(self, adv):
        "He died"
        self.adventurers.remove(adv)

    def new_turn(self, new_world):
        "New turn"
        self._generate_objectives(new_world)
        # Reset objectives for adventurers
        for adv in self.adventurers:
            adv.select_random_objective(self.objectives)
    
        bids = []
        for i in range(TABLE_TURNS):
            # Find bidders
            bids = [ adv.communicate() for adv in self.adventurers]
            bids = [ a[1] for a in bids if a[0] == 2] # Filter only bids
            bids.sort(key=lambda bid : bid.wage, reverse = True) # Sort by best bider
            if len(bids) == 0: # No more bider, end of turn
                break
            # Find acceptors
            acceptors = [ (adv,adv.communicate(bids)) for adv in self.adventurers]
            acceptors = [ (a[0], a[1][1]) for a in acceptors if a[1][0] == 1 ] # Filter only acceptors
            if len(acceptors) == 0: # No more acceptors, end of turn
                break
            acceptor = random.choice(acceptors)
            # Accept bid
            bid = acceptor[1]
            bid.accept(acceptor[0])
        return self.adventurers

    def _generate_objectives(self, world):
        "Generate a list of objectives on the map and store it"
        self.objectives = []
        for x in range(world.get_size()[0]):
            for y in range(world.get_size()[1]):
                cell = world.get_cell(x, y)
                if cell["human"] != 0:
                    #self.objectives.append(("human", (x, y), cell["human"]))
                    obj = Objective("human", (x,y), cell["human"])
                    self.objectives.append(obj)
                if cell["ennemy"] != 0:
                    #self.objectives.append(("ennemy", (x, y), cell["ennemy"]))
                    obj = Objective("ennemy", (x,y), cell["ennemy"])
                    self.objectives.append(obj)
                    
    def turn_callback(self, world):
        # Call back when its our turn to play
        # First check if an adventure died
        to_remove = []
        for adv in self.adventurers:
            if world.get_cell(adv.pos[0], adv.pos[1])["us"] == 0:
                # Kill adventurer
                to_remove.append(adv)
        for adv in to_remove:
            self.del_adventurer(adv)
        print "################"
        print "### NEW TURN ###"
        adventurers = self.new_turn(world)
        moves = []
        poses = []
        for adv in adventurers:
            new_pos = world.find_path(adv.pos, adv.objective.pos)
            if new_pos in poses:
                print "Illegal move for adventurer:",adv
                continue
            else:
                poses.append(adv.pos)
                move = (adv.pos[0], adv.pos[1], 1, new_pos[0], new_pos[1])
                adv.pos = new_pos
                moves.append(move)
        # Squash moves
        mm = {}
        for m in moves:
            key = (m[0:2], m[3:5])
            if mm.has_key(key):
                mm[key] += 1
            else:
                mm[key] = 1
        moves = []
        for key in mm.keys():
            moves.append((key[0][0],key[0][1], mm[key], key[1][0], key[1][1]))
        #if len(moves) > 3: moves = moves[:3]
        # End of turn everyone has something to do, print it
        print "### END OF TURN ###"
        for adv in self.adventurers:
            print adv
        print "Moves:",moves
        print "###################"
        time.sleep(0.5) # Wait cause our AI is way too fast :)
        self.api.move(moves)
        
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
    name = names.get_first_name()
    print "You are playing as", name
    try:
        c.connect(name)
    except:
        print "Couldn't connect to server..."
        wait = raw_input("Press return to exit...")
        sys.exit()
    time.sleep(0.2)
    pos = find_starting(c.get_map())
    world = c.get_map()
    hall = GamingHall(world, c)
    start = find_starting(world)
    number = c.get_map().get_cell(start[0], start[1])["us"]
    # Create 4 adventurers
    for i in range(number):
        adv = Adventurer(hall, start)
        hall.add_adventurer(adv)
    # Play one turn
    #while 1:
        #hall.new_turn(world)
    wait = raw_input("...")
    
    
