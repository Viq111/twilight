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

inf = float('infinity')

###############
### CLASSES ###
###############
cdef class MinMax:
    "A class to do a MinMax algorithm on a game"

    cdef public object game
    cdef public int depth

    def  __init__(self, object game, int depth):
        "Do a MinMax on a game"
        self.game = game
        self.depth = depth

    def  ask_move(self):
        "Ask best move"
        moves = self.ask_moves()
        if len(moves) > 0:
            return moves[0][1]
        else:
            return None # No move possible

    def ask_moves(self):
        "Return a list of moves and their score"
        game = self.game
        if game.nplayer != 1: # Force to be player 1 first
            game.switch_player()
        scores = []
        moves = game.possible_moves()
        cdef float score
        for move in moves:
            game.make_move(move)
            score = self._minmax()
            scores.append((score, move))
            game.unmake_move(move)
        scores.sort(reverse=True) # Sort by best score
        return scores


    cdef float _minmax(self, int depth=0, float alpha=-inf, float beta=inf):
        "Do a minmax algorithm with alpha-beta pruning"
        cdef float localAplha = alpha
        cdef float localBeta = beta
        game = self.game
        game.switch_player() # It's the other player time to play
        if game.is_over() or depth >= self.depth:
            game.switch_player()
            return game.scoring()
        moves = game.possible_moves()
        cdef float score
        if game.nplayer == 1:  # max player
            score = localAplha
            for move in moves:
                game.make_move(move)
                localAplha = max(localAplha, self._minmax(depth + 1, localAplha, localBeta))
                score = localAplha
                game.unmake_move(move)
                if localAplha >= localBeta:
                    score = localBeta
                    break
        else:  # min player
            score = localBeta
            for move in moves:
                game.make_move(move)
                localBeta = min(beta, self._minmax(depth + 1, localAplha, localBeta))
                score = localBeta
                game.unmake_move(move)
                if localAplha >= localBeta:
                    score = localAplha
                    break
        game.switch_player() # Go back to previous player
        return score


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
        
