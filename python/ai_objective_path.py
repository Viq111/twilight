# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "AI - Objective Path"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time
import easyAI
import client_api

###############
### GLOBALS ###
###############

MINMAX_LEVEL = 4
PENALITY_COEFF = 2

###############
### CLASSES ###
###############

class Objective():
    "An objective"
    def __init__(self, position, number):
        "An objective is a human camp"
        self.pos = position
        self.nb = number
    def __repr__(self):
        return "<" + str(self.nb) + " at " + str(self.pos) + ">"

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
        self.nb_penalities = 0 # Nubmer of penalities because we are farther from the objective relative to the ennemy

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
        moves = [ (False, move) for move in moves ] # First boolean is if we take a penality
        if len(moves) == 0:
            # We are farther from all objectives relating to the ennemy, take a penality
            moves = filter(lambda obj : obj.nb <= p[1], self.free_objectives) # Only where I can attack safely
            moves = [ (True, move) for move in moves ] # First boolean is if we take a penality
        return moves # Possible objective I can go for

    def make_move(self, move):
        "Make a move to the objective"
        obj = move[1]
        if move[0]: # We took a penality
            self.nb_penalities += 1
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
            self.nb_penalities -= 1
        if obj == None: # Stuck move
            self.stuck = False
            return
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
        # Score is nb / nb_moves of us minus ennemy
        p1_score = 1.0 * self.p1_obj[-1][1] / self.p1_obj[-1][2]
        p2_score = 1.0 * self.p1_obj[-1][1] / self.p1_obj[-1][2]
        if self.nplayer == 1:
            return p1_score - p2_score - (self.nb_penalities * PENALITY_COEFF)
        else:
            return p2_score - p1_score - (self.nb_penalities * PENALITY_COEFF)
    def show(self):
        return

class Objective_path():
    "An AI which tries to find the best path between objectives"
    def __init__(self, client):
        "Connect to the client"
        client.set_callback(self.callback)
        client.connect("Path")
        self.c = client
        self.ai = easyAI.AI_Player(easyAI.Negamax(MINMAX_LEVEL))
        self.ennemy = easyAI.AI_Player(easyAI.Negamax(MINMAX_LEVEL))

    def callback(self, world):
        "Play nearest objective"
        # 0 - Update our number and position and ennemy
        us = world.get_starting_position()
        us = (us, world.get_cell(us[0], us[1])["us"], 0)
        ennemy = self._get_ennemy_best_position(world)
        ennemy = (ennemy, world.get_cell(ennemy[0], ennemy[1])["ennemy"], 0)
        size = world.get_size()
        
        # 1 - Find objectives
        objectives = []
        for x in range(size[0]):
            for y in range(size[1]):
                if world.get_cell(x, y)["human"] != 0:
                    obj = Objective((x, y), world.get_cell(x, y)["human"])
                    objectives.append(obj)
        if len(objectives) > 0: # There is still humans on the map
            # Ask AI solver what he wants to play
            game = BoardGame([self.ai, self.ennemy], us, ennemy, objectives)
            move = self.ai.ask_move(game)[1]
            goal = world.find_path(us[0], move.pos)
            print("[AI] Going for humans at " + str(move.pos) + " through " + str(goal))
            self.c.move([(us[0][0], us[0][1], us[1], goal[0], goal[1])])
        else: # No more humans, attack the other player
            ennemies = []
            for x in range(size[0]):
                for y in range(size[1]):
                    if world.get_cell(x, y)["ennemy"] != 0:
                        objectives.append((x, y, world.get_cell(x, y)["ennemy"]))
            # Find best pray
            objectives.sort(key = lambda e : e[2])
            goal = world.find_path(us[0], (objectives[0][0], objectives[0][1]))
            print("[AI] Going for ennemy at " + str(objectives[0]) + " through " + str(goal))
            self.c.move([(us[0][0], us[0][1], us[0][1], goal[0], goal[1])])


    def _get_ennemy_best_position(self, world):
        "Get the ennemy position where he has the most troups"
        ennemies = []
        size = world.get_size()
        for col in range(size[0]):
            for line in range(size[1]):
                if world.get_cell(col, line)["ennemy"] != 0:
                    ennemies.append((world.get_cell(col, line)["ennemy"], (col, line)))
        ennemies.sort(reverse=True)
        return ennemies[0][1]
        
        
###################
### DEFINITIONS ###
###################

def main(client):
    "Create an AI and play"
    ai = Objective_path(client)
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
