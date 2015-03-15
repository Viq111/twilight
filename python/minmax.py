# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "MinMax Algorithm"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time, sys

###############
### GLOBALS ###
###############

###############
### CLASSES ###
###############

class MinMax():
    "A class to do a MinMax algorithm on a game"
    def __init__(self, game, depth):
        "Do a MinMax on a game"
        self.game = game
        self.depth = depth

    def ask_move(self):
        "Ask best move"
        moves = self.ask_moves()
        return moves[0][1]

    def ask_moves(self):
        "Return a list of moves and their score"
        game = self.game
        if game.nplayer != 1: # Force to be player 1 first
            game.switch_player()
        scores = []
        moves = game.possible_moves()
        for move in moves:
            game.make_move(move)
            score = self._minmax()
            scores.append((score, move))
            game.unmake_move(move)
        scores.sort(reverse=True) # Sort by best score
        return scores

    def _minmax(self, depth = 0):
        "Do a minmax algorithm"
        game = self.game
        game.switch_player() # It's the other player time to play
        player = game.nplayer
        if game.is_over() or depth >= self.depth:
            game.switch_player()
            return game.scoring()
        scores = []
        moves = game.possible_moves()
        for move in moves:
            game.make_move(move)
            scores.append(self._minmax(depth + 1))
            game.unmake_move(move)
        game.switch_player() # Go back to previous player
        if game.nplayer != 1: # If it was player 1
            return max(scores)
        else: # Else it was second player
            return min(scores)
            

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
        
