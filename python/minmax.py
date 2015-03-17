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

class MinMax():
    "A class to do a MinMax algorithm on a game"
    def __init__(self, game, depth):
        
        "Do a MinMax on a game"
        self.game = game
        self.depth = depth

    def ask_move(self):
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
        for move in moves:
            game.make_move(move)
            score = self._minmax()
            scores.append((score, move))
            game.unmake_move(move)
        scores.sort(reverse=True) # Sort by best score
        return scores

    def _minmax(self, depth=0, alpha=-inf, beta=inf):
        "Do a minmax algorithm with alpha-beta pruning"
        game = self.game
        game.switch_player() # It's the other player time to play
        if game.is_over() or depth >= self.depth:
            game.switch_player()
            return game.scoring()
        moves = game.possible_moves()
        if game.nplayer == 1:  # max player
            score = alpha
            for move in moves:
                game.make_move(move)
                alpha = max(alpha, self._minmax(depth + 1, alpha, beta))
                score = alpha
                game.unmake_move(move)
                if alpha >= beta:
                    score = beta
                    break
        else:  # min player
            score = beta
            for move in moves:
                game.make_move(move)
                beta = min(beta, self._minmax(depth + 1, alpha, beta))
                score = beta
                game.unmake_move(move)
                if alpha >= beta:
                    score = alpha
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
        
