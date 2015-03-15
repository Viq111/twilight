# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "AI - Pylight"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time, traceback, copy
import client_api
import minmax

###############
### GLOBALS ###
###############

MINMAX_LEVEL = 4
FAST_MINMAX_LEVEL = 2
PENALITY_COEFF = 2
DEBUG = True # In production, remove Debug so error are caught and a dumbed down version is used

###############
### CLASSES ###
###############

class WorldHelper():
    "Class for some helper functions about the world with optimisations"
    def __init__(self, world):
        "Store the world and previous computations"
        self.world = world
        self.pos_humans = None
        self.pos_us = None
        self.pos_ennemies = None

    def get_humans(self):
        "Get position of humans as a list ((x, y), number)"
        if self.pos_humans:
            return self.pos_humans
        size = self.world.get_size()
        self.pos_humans = []
        for i in range(size[0]):
            for j in range(size[1]):
                cell = self.world.get_cell(i, j)
                if cell["human"] != 0:
                    self.pos_humans.append(((i, j), cell["human"]))
        return self.pos_humans

    def get_ennemies(self):
        "Get position of ennemies as a list"
        if self.pos_ennemies:
            return self.pos_ennemies
        size = self.world.get_size()
        self.pos_ennemies = []
        for i in range(size[0]):
            for j in range(size[1]):
                cell = self.world.get_cell(i, j)
                if cell["ennemy"] != 0:
                    self.pos_ennemies.append(((i, j), cell["ennemy"]))
        return self.pos_ennemies

    def get_nearest_ennemy(self, pos):
        "Get nearest ennemy from us"
        # List of (distance, group of ennemies)
        dist = [ (self._flight_distance(pos, e[0]), e) for e in self.get_ennemies() ]
        dist.sort() # Nearest ennemy
        if len(dist) == 0: # No ennemy
            return None
        else:
            return dist[0][1] # Return nearest ennemy

    def get_biggest_ennemy(self):
        "Get the biggest ennemy"
        ennemies = self.get_ennemies()
        ennemies.sort(key = lambda e : e[1], reverse=True) # Sort by biggest number
        if len(ennemies) == 0:
            return None
        else:
            return ennemies

    def _flight_distance(self, pos1, pos2):
        "Distance from pos1 to pos2"
        return max(abs(pos1[0] - pos2[0]), abs(pos1[1] - pos2[1]))

class Objective():
    "An objective"
    def __init__(self, position, number):
        "An objective is a human camp"
        self.pos = position
        self.nb = number
    def __repr__(self):
        return "<" + str(self.nb) + "units at " + str(self.pos) + ">"

    def __eq__(self, other):
        "Check if objectives are the same"
        return (self.pos == other.pos) and (self.nb == other.nb)

class BoardGame():
    "Create a simuation of the board game"
    def __init__(self, player1, player2, objectives):
        "Store information about the player and objectives"
        self.nplayer = 1
        # Store
        self.p1_obj = [player1] # P1 objectives he got, this is a list of ((current_x, current_y), total_nb, nb_moves)
        self.p2_obj = [player2] # P2 objectives he got, this is a list of ((current_x, current_y), total_nb, nb_moves)
        self.free_objectives = objectives

    # Helper functions
    
    def switch_player(self):
        "Switch players"
        if self.nplayer == 1:
            self.nplayer = 2
        else:
            self.nplayer = 1

    def __flight_distance(self, start, stop):
        "Flight distance to go from start to stop"
        return max(abs(start[1] - stop[1]), abs(start[0] - stop[0]))

    def possible_moves(self):
        "Possible moves for current player"
        # I can move only on objectives where I'm nearer than the ennemy
        if self.nplayer == 1: # P1 playing
            p = self.p1_obj[-1]
            e = self.p2_obj[-1]
        else:
            p = self.p2_obj[-1]
            e = self.p1_obj[-1]
        moves = filter(lambda obj : obj.nb <= p[1], self.free_objectives) # Only where I can attack safely
        moves = filter(lambda obj : self.__flight_distance(p[0], obj.pos) <= self.__flight_distance(e[0], obj.pos), moves) # Only where I can be before the ennemy
        moves = [ (False, move) for move in moves ] # First boolean is if we take a penality
        if len(moves) == 0:
            # We are farther from all objectives relating to the ennemy, take a penality
            moves = filter(lambda obj : obj.nb <= p[1], self.free_objectives) # Only where I can attack safely
            moves = [ (True, move) for move in moves ] # First boolean is if we take a penality
        return moves # Possible objective I can go for

    def make_move(self, move):
        "Make a move to the objective"
        obj = move[1]
        if self.nplayer == 1: # P1 playing
            p = self.p1_obj[-1]
        else:
            p = self.p2_obj[-1]
        nb = p[1] + obj.nb
        nb_moves = p[2] + self.__flight_distance(p[0], obj.pos)
        if move[0]: # We took a penality
            nb_moves = p[2] + (self.__flight_distance(p[0], obj.pos) * PENALITY_COEFF)
        else:
            nb_moves = p[2] + self.__flight_distance(p[0], obj.pos)

        self.free_objectives.remove(obj)
        if self.nplayer == 1: # P1 playing
            self.p1_obj.append((obj.pos, nb, nb_moves))
        else:
            self.p2_obj.append((obj.pos, nb, nb_moves))

    def unmake_move(self, move):
        "Unmake a move"
        obj = move[1]
        if self.nplayer == 1:
            del self.p1_obj[-1]
        else:
            del self.p2_obj[-1]
        self.free_objectives.append(obj)

    def is_over(self):
        "Tell if game is over"
        if len(self.free_objectives) == 0:
            return True # No more objectives, nothing to do
        humans = min(h.nb for h in self.free_objectives) # Min number of humans
        if humans > self.p1_obj[-1][1] or humans > self.p2_obj[-1][1]: # If we can't safely tank humans, end the MinMax search
            return True
        return False

    def scoring(self):
        "Return a score of current player"
        # Score is (nb / nb_moves of us) minus ennemy's score
        if self.p1_obj[-1][2] ==0: # No move, no gain
            p1_score = 0
        else:
            p1_score = (1.0 * self.p1_obj[-1][1] / self.p1_obj[-1][2])
        if self.p2_obj[-1][2] ==0:
            p2_score = 0  
        else:
            p2_score = (1.0 * self.p2_obj[-1][1] / self.p2_obj[-1][2])
        return p1_score - p2_score
            
    def show(self):
        return

class PylightParty():
    "A party of units"
    def __init__(self, parent = None):
        "Create a new party splitted from a previous one"
        self.grouping = False # Once we need to group with our parent, set that to true
        self.parent = parent

    # Internal
    def get_id(self):
        "This will be used if we use multiprocessing, return an id defining the object"
        return self

    # Core

    def select_moves(self, pos, nb, world):
        "Update current pos and nb. Select a list of best moves and return them (score, move, group division). move is (pos, nb, goal). Group division is (pos, nb). If want to group with parent, return parent"
        if self.grouping: # We want to group with parent
            return self.parent
        wh = WorldHelper(world)
        # Us
        us = (pos, nb, 0) # This is ((init_x, init_y), nb_units, nb_moves)
        # Ennemy
        ennemy = wh.get_biggest_ennemy() # ToDo: Might change that to nearest ennemy
        ennemy = (ennemy[0][0], ennemy[0][1], 0) # (Pos, nb, moves)
        size = world.get_size()
        
        # 1 - Find objectives
        objectives = []
        for h in wh.get_humans():
            objectives.append(Objective(h[0], h[1]))
        if len(objectives) > 0: # There is still humans on the map
            # Ask AI solver what he wants to play
            game = BoardGame(us, ennemy, objectives)
            start = time.time()
            mm = minmax.MinMax(game, MINMAX_LEVEL)
            try:
                move = mm.ask_move()
            except: # If not on DEBUG, print bug but use dumbed down version
                if DEBUG:
                    raise
                else:
                    print traceback.format_exc()
            if move: # If we can do a move
                move = move[1]
                print("[PERF] Computation took " + str(int((time.time()-start)*100)/100.0) + "secs")
                goal = world.find_path(us[0], move.pos)
                print("[AI](" + str(us[1]) + ") Going for humans at " + str(move.pos) + " through " + str(goal))
                # Try to detect if we can fork
                group_move = (us[0], us[1], move.pos)
                group_objectives = copy.deepcopy(objectives)
                group_objectives.remove(move)
                groups = self.check_group(world, group_move, ennemy, group_objectives)
                if groups:
                    print("[FORK] Want to create groups: " + str(groups))
                return [(1, (us[0], us[1], goal))] # Currently score is 1
        # Either there is no more objectives on the map or the objective are too high
        if self.parent != None:
            # We are a child, we are cowards, so let's group
            self.grouping = True
            return self.parent
        # Detect the best pray
        ennemies = wh.get_ennemies()
        ennemies.sort(key = lambda e : e[1])
        ennemy = ennemies[0] # (pos, nb)
        humans = wh.get_humans()
        humans.sort(key = lambda e : e[1])
        if len(humans) == 0: # If no more ennemy, go for the ennemy
            go_for_ennemy = True
        else: # Else compute static to win against human or ennemy
            if ennemy[0][1] <= humans[0][1]: # It's better to attack the ennemy
                go_for_ennemy = True
            else:
                go_for_ennemy = False
        if go_for_ennemy:
            goal = world.find_path(us[0], ennemy[0])
            print("[AI](" + str(us[1]) + ") Going for ennemy at " + str(ennemy[0]) + " through " + str(goal))
        else:
            goal = world.find_path(us[0], humans[0][0])
            print("[AI](" + str(us[1]) + ") Going for human at " + str(humans[0][0]) + " through " + str(goal))
        return [(1, (us[0], us[1], goal))] # Currently score is 1

    def check_group(self, world, move, ennemy, objectives, parent = None):
        "For a move check if we can create a group. Returns None is not possible or a tuple of units, list of (tuple of (nb, Pylight party)) (move is (pos, nb, objective_pos))"
        if parent == None: # If parent was not given, take it from the object
            parent = self.parent
        # Move is (pos, nb, objective_pos)
        objective_size = world.get_cell(move[2])
        objective_size = objective_size["human"] + objective_size["ennemy"] # The objective is either a human or an ennemy
        remaining_units = move[1] - objective_size

        # Ok now, try to find if there are sensible moves
        us = (move[0], remaining_units, 0)
        game = BoardGame(us, ennemy, objectives)
        mm = minmax.MinMax(game, FAST_MINMAX_LEVEL)
        obj = mm.ask_move()
        if not obj: # Do not need to do anything
            return None
        obj = obj[1] # Only take objective not penality
        # OK, this is good, let's create a party
        party = PylightParty(parent)
        # Find by recurrence if we can create another party
        objectives2 = copy.deepcopy(objectives)
        objectives2.remove(obj)
        move2 = (move[0], remaining_units, obj.pos) # (pos, nb, objective_pos)
        other_groups = self.check_group(world, move2, ennemy, objectives2, party.get_id())
        if not other_groups: # No other groups, just return us
            return [(remaining_units, party)]
        else:
            to_remove = 0 # Nb to remove from curent party
            for group in other_groups:
                to_remove += group[0]
            return [(remaining_units - to_remove, party)] + other_groups

class PylightAI():
    "An AI which tries to take the best world of all our AIs. One AI to rule them all."

    name = "Pylight"
    
    def __init__(self, client):
        "Store the client"
        self.c = client
        # Create one PyParty
        self.master = PylightParty()

    def callback(self, world):
        "Play best objective"
        pos = world.get_starting_position() # Get our only position
        nb = world.get_cell(pos)["us"]
        moves = self.master.select_moves(pos, nb, world)
        move = moves[0][1] # Take only first move
        self.c.move([(move[0][0], move[0][1], move[1], move[2][0], move[2][1])])

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
    with client_api.ClientAPI() as client:
        client.mainloop(PylightAI)
