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
import os, time
import client_api
import easyAI # ToDo: Remove this

###############
### GLOBALS ###
###############

MINMAX_LEVEL = 8
PENALITY_COEFF = 2

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

class BoardGame(easyAI.TwoPlayersGame):
    "Create a simuation of the board game"
    def __init__(self, players, player1, player2, objectives):
        "Store information about the player and objectives"
        # easyAI initialization
        self.players = players
        self.nplayer = 1
        # Store
        self.p1_obj = [player1] # P1 objectives he got, this is a list of ((current_x, current_y), total_nb, nb_moves)
        self.p2_obj = [player2] # P2 objectives he got, this is a list of ((current_x, current_y), total_nb, nb_moves)
        self.free_objectives = objectives
        self.p1_penalities = 0 # Nubmer of penalities because we are farther from the objective relative to the ennemy
        self.p2_penalities = 0 # Nubmer of penalities because we are farther from the objective relative to the ennemy

    # Helper functions
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
        moves = [ (0, move) for move in moves ] # First boolean is if we take a penality
        if len(moves) == 0:
            # We are farther from all objectives relating to the ennemy, take a penality
            moves = filter(lambda obj : obj.nb <= p[1], self.free_objectives) # Only where I can attack safely
            moves = [ (1, move) for move in moves ] # First boolean is if we take a penality
        if len(moves) == 0:
            # We are not numerous enought, go random
            moves = [ (10, move) for move in self.free_objectives ]
        return moves # Possible objective I can go for

    def make_move(self, move):
        "Make a move to the objective"
        obj = move[1]
        if move[0]: # We took a penality
            if self.nplayer == 1:
                self.p1_penalities += move[0]
            else:
                self.p2_penalities += move[0]

        if self.nplayer == 1: # P1 playing
            p = self.p1_obj[-1]
        else:
            p = self.p2_obj[-1]
        nb = p[1] + obj.nb
        nb_moves = p[2] + self.__flight_distance(p[0], obj.pos)
        self.free_objectives.remove(obj)
        if self.nplayer == 1: # P1 playing
            self.p1_obj.append((obj.pos, nb, nb_moves))
        else:
            self.p2_obj.append((obj.pos, nb, nb_moves))

    def unmake_move(self, move):
        "Unmake a move"
        obj = move[1]
        if move[0]: # We took a penality
            if self.nplayer == 1:
                self.p1_penalities -= move[0]
            else:
                self.p2_penalities -= move[0]

        if self.nplayer == 1:
            del self.p1_obj[-1]
        else:
            del self.p2_obj[-1]
        self.free_objectives.append(obj)

    def is_over(self):
        "Tell if game is over"
        return len(self.free_objectives) == 0

    def scoring(self):
        "Return a score of current player"
        if self.nplayer == 1 and len(self.p1_obj) == 1: # No move, no gain
            return 0
        if self.nplayer == 2 and len(self.p2_obj) == 1:
            return 0
        # Score is (nb / nb_moves of us) minus ennemy's score
        p1_score = (1.0 * self.p1_obj[-1][1] / self.p1_obj[-1][2]) - (self.p1_penalities * PENALITY_COEFF)
        p2_score = (1.0 * self.p2_obj[-1][1] / self.p2_obj[-1][2]) - (self.p2_penalities * PENALITY_COEFF)
        return p1_score - p2_score
            
    def show(self):
        return

class PylightParty():
    "A party of units"
    def __init__(self, pos, nb, parent = None):
        "Create a new party splitted from a previous one"
        self.grouping = False # Once we need to group with our parent, set that to true
        self.parent = parent
        # ToDo: Remove when new MiniMax is available
        self.ai = easyAI.AI_Player(easyAI.Negamax(MINMAX_LEVEL))
        self.ennemy = easyAI.AI_Player(easyAI.Negamax(MINMAX_LEVEL))
        
    def select_moves(self, pos, nb, world):
        "Update current pos and nb. Select a list of best moves and return them (score, move). move is (pos, nb, goal)"
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
            game = BoardGame([self.ai, self.ennemy], us, ennemy, objectives)
            start = time.time()
            move = self.ai.ask_move(game)[1]
            print("[PERF] Computation took " + str(int((time.time()-start)*100)/100.0) + "secs")
            goal = world.find_path(us[0], move.pos)
            print("[AI](" + str(us[1]) + ") Going for humans at " + str(move.pos) + " through " + str(goal))
            return [(1, (us[0], us[1], goal))] # Currently score is 1
            #self.c.move([(us[0][0], us[0][1], us[1], goal[0], goal[1])])
        else: # No more humans, attack the other player
            ennemies = wh.get_ennemies()
            ennemies.sort(key = lambda e : e[1])
            goal = world.find_path(us[0], ennemies[0][0])
            print("[AI](" + str(us[1]) + ") Going for ennemy at " + str(ennemies[0][0]) + " through " + str(goal))
            return [(1, (us[0], us[1], goal))] # Currently score is 1
            #self.c.move([(us[0][0], us[0][1], us[1], goal[0], goal[1])])
        

class PylightAI():
    "An AI which tries to take the best world of all our AIs. One AI to rule them all."

    name = "Pylight"
    
    def __init__(self, client):
        "Store the client"
        self.c = client
        world = self.c.get_world()
        pos = world.get_starting_position()
        nb = world.get_cell(pos)
        # Create one PyParty
        self.master = PylightParty(pos, nb)

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
