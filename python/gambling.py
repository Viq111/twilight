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

TABLE_TURNS = 5

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
        self.quest = None

    def new_turn(self):
        "Inform the adventurer that it is a new turn"
        self.quest = None
        
    def select_quest(self, world, quests):
        "Examine all quests and select the best one, return the selected quest"
        if self.quest != None:
            current_score = self.quest.get_current_benefit() * 1.0 / world.find_path_time(self.pos, self.quest.objective.pos)
        else:
            current_score = -1000000
        bests = [ (quest.get_benefit(self.pos), quest) for quest in quests ]
        bests.sort(key=lambda q : q[0], reverse=True)
        if bests[0][0] > current_score:
            return bests[0][1]
        else:
            return self.quest

    def change_quest(self, new_quest):
        "Accept (forced) a new quest"
        if self.quest != None: # Deenroll from previous quest
            self.quest.deserted_by(self)
        self.quest = new_quest
        self.quest.accepted_by(self) # Enroll

    def __repr__(self):
        "Return a representation of the adventurer"
        s  = str(self.name) + " > Quest: " + str(self.quest)
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

    def get_reward(self, number):
        "Get reward for the number of adventurers"
        for i in range(number, 0, -1):
            if self.tier.get(i, None) != None:
                return self.tier[i]
        return 0
            
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
        # ToDo: Add cost to lose a minion
        for i in range(1, critic):
            reward = 90 * number * i / (i+number)
            tier[i] = reward
        return tier

    def __repr__(self):
        "Show an objetive"
        s = "destroy " + str(self.obj) + " (" + str(self.number) + ") at position " + str(self.pos)
        return s

class Quest():
    "A quest consist of an objective, a list of adventurer and a meeting point"
    def __init__(self, world, objective):
        "Create a quest with no assigned adventurer"
        self.world = world
        self.objective = objective
        self.adventurers = []
        self.moves = {} # Associate a adventurer to a number of move he needs

    def __repr__(self):
        "Show some information about the quest"
        s = self.objective.__repr__()
        if len(self.adventurers) >= 2:
             s += " with " + str(len(self.adventurers) -1) + " other adventurer(s)"
        else:
            s += " alone"
        return s

    def get_minimal_moves(self):
        "Give the number of minimal moves for all adventurers to go there"
        if len(self.moves) > 0:
            return min(self.moves.values())
        else:
            return 0
        
    def get_benefit(self, current_position):
        "Get the benefit score if the adventurer joins"
        benef = 0
        nb_moves = self.world.find_path_time(current_position, self.objective.pos)
        # If nb_moves > minimal_moves, remove the cost from other adventurers
        if len(self.adventurers) > 0:
            if nb_moves > self.get_minimal_moves():
                benef -=  self.objective.get_reward(len(self.adventurers)) * 1.0 * (nb_moves - self.get_minimal_moves()) / self.get_minimal_moves()
        benef += self.objective.get_reward(len(self.adventurers)+1) - self.objective.get_reward(len(self.adventurers))
        return benef

    def get_current_benefit(self):
        "Get current benefit"
        if len(self.adventurers) >= 2:
            return self.objective.get_reward(len(self.adventurers)) - self.objective.get_reward(len(self.adventurers)-1)
        else:
            return self.objective.get_reward(len(self.adventurers))

    def accepted_by(self, adventurer):
        "The quest is accepted by a new adventurer"
        self.adventurers.append(adventurer)
        self.moves[adventurer] = self.world.find_path_time(adventurer.pos, self.objective.pos)

    def deserted_by(self, adventurer):
        "The quest is abandoned by an adventurer"
        self.adventurers.remove(adventurer)
        del self.moves[adventurer]

    def get_meeting_point(self):
        "If all adventurers would not arrive at the same time, create a meeting point next to the goal, else return the goal position"
        if len(set(self.moves.values())) <= 1: # All adventurers are going to arrive at the same time
            return self.objective.pos
        else: # Compute the nearest meeting point for the slowest member
            slow = [ (self.moves[adv], adv) for adv in self.moves.keys() ]
            slow.sort(reverse=True)
            slowest = slow[0][1] # Slowest adventurer
            path = self.world._a_star(slowest.pos, self.objective.pos) # His path to get there
            if len(path) >= 2:
                return path[-2] # Return the move just before arriving to the goal
            else: # This is the slowest, don't move
                return slowest.pos
        

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
        for adv in self.adventurers:
            adv.new_turn()
        # Create quests
        objectives = self._generate_objectives(new_world)
        quests = [ Quest(new_world, o) for o in objectives ]
        # Shuffle adventurers so each one has a chance to choose first
        random.shuffle(self.adventurers)
        for i in range(TABLE_TURNS):
            for adv in self.adventurers:
                new_quest = adv.select_quest(world, quests)
                adv.change_quest(new_quest)
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
        return self.objectives
    
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
        # Then check if we have new adventurers
        # Create a dict of nb adv per cell
        already = {}
        for adv in self.adventurers:
            if not already.has_key(adv.pos):
                already[adv.pos] = 1
            else:
                already[adv.pos] += 1
        # Then check those position
        for pos in already.keys():
            diff = world.get_cell(pos[0], pos[1])["us"] > already[pos]
            if diff:
                for i in range(diff): # For each converted human, create an adventurer
                    adv = Adventurer(self, pos)
                    self.adventurers.append(adv)
        print "################"
        print "### NEW TURN ###"
        adventurers = self.new_turn(world)
        moves = []
        poses = [adv.pos for adv in adventurers] # Get all current positions of adventurers
        for adv in adventurers:
            #new_pos = world.find_path(adv.pos, adv.quest.objective.pos)
            new_pos = world.find_path(adv.pos, adv.quest.get_meeting_point())
            if new_pos == adv.pos: # Not moving, don't do anything
                pass
            else:
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
    
    
