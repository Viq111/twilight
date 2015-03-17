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
import os, time, traceback, copy, argparse, sys
import termcolor
import client_api
import minmax

from pprint import pprint

###############
### GLOBALS ###
###############

INITIAL_MINMAX_LEVEL = 2
INITIAL_FAST_MINMAX_LEVEL = 2
PENALITY_COEFF = 2
DEBUG = False # In production, remove Debug so error are caught and a dumbed down version is used

# DO NOT TOUCH
PIRATIONAL_CONST = 0

# Print definition

def print_main(*args):
    "Print thing with main color"
    COLOR = "magenta"
    s = "[MAIN] "
    for a in args:
        s += str(a)
    termcolor.cprint(s, COLOR)

def print_party(*args):
    "Print with party color"
    COLOR = "cyan"
    s = "[PARTY] "
    for a in args:
        s += str(a)
    termcolor.cprint(s, COLOR)

def print_perf(*args):
    "Print with perf color"
    COLOR = "blue"
    s = "[PERF] "
    for a in args:
        s += str(a)
    termcolor.cprint(s, COLOR)

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

    def get_us(self):
        "Get all our positions"
        if self.pos_us:
            return self.pos_us
        size = self.world.get_size()
        self.pos_us = []
        for i in range(size[0]):
            for j in range(size[1]):
                cell = self.world.get_cell(i, j)
                if cell["us"] != 0:
                    self.pos_us.append(((i, j), cell["us"]))
        return self.pos_us

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

    def __repr__(self):
        return "<Party " + str(id(self)) + ">"

    # Core

    def select_moves(self, pos, nb, world, level, fast_level):
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
            mm = minmax.MinMax(game, level)
            try:
                moves = mm.ask_moves()
            except: # If not on DEBUG, print bug but use dumbed down version
                if DEBUG:
                    raise
                else:
                    print traceback.format_exc()
            #print_perf("Computation took " + str(int((time.time()-start)*100)/100.0) + "secs")
            if len(moves) > 0: # If we can do a move
                result = []
                for m in moves: # m is (score, move) and move is (penality, obj)
                    # Try to detect if we can fork
                    group_move = (us[0], us[1], m[1][1].pos)
                    group_objectives = copy.deepcopy(objectives)
                    group_objectives.remove(m[1][1])
                    groups = self.check_group(world, group_move, ennemy, group_objectives, fast_level, parent=self.get_id())
                    result.append((m[0], (us[0], us[1], m[1][1].pos), groups)) # (score, move, group division) with move = (pos, nb, goal) 
                return result
        # Either there is no more objectives on the map or the objective are too high
        if self.parent != None:
            # We are a child, we are cowards, so let's group
            self.grouping = True
            print_party("Wanting to regroup")
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
            goal = ennemy[0]
        else:
            goal = humans[0][0]
        return [(PIRATIONAL_CONST, (us[0], us[1], goal), None)] # Score of PIRATIONAL_CONST means we either attack smth or wait for our children

    def check_group(self, world, move, ennemy, objectives, level, parent = None):
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
        mm = minmax.MinMax(game, level)
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
        other_groups = self.check_group(world, move2, ennemy, objectives2, level, party.get_id())
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
        pos = self.c.get_world().get_starting_position()
        nb = self.c.get_world().get_cell(pos)
        # Create one PyParty
        self.parties = [PylightParty()]
        self.tracking = {} # Associate a party to its old position (in case of) and size and current supposed position
        self.tracking[self.parties[0]] = (pos, nb, pos) # And set the start position of our group
        self.blocked = False # becomes True once the the total computation time has reached 1 sec at least once
        self.minmax_level = INITIAL_MINMAX_LEVEL
        self.fast_minmax_level = INITIAL_FAST_MINMAX_LEVEL

    def _update_tracking(self, world):
        "Update traking of differents parties with the new world"
        # First try to detect all positions and number of each of our groups
        wh = WorldHelper(world)
        us = wh.get_us()
        us = dict(us) # Transform to {(x,y) -> nb} dict
        new_tracking = {}
        not_found = {} # Parties not found
        # First put all units in their previous group
        for p in self.tracking.keys():
            nb = self.tracking[p][1]
            pos = self.tracking[p][2]
            old_pos = self.tracking[p][0]
            if us.get(pos, 0) < nb:
                not_found[p] = self.tracking[p]
            else:
                new_tracking[p] = (pos, nb)
                us[pos] -= nb
        # Then try to find not found parties
        for p in not_found.keys():
            nb = self.tracking[p][1]
            pos = self.tracking[p][2]
            old_pos = self.tracking[p][0]
            ## Can be because we lost some members
            if us.get(pos, 0) > 0:
                new_tracking[p] = (pos, us.get(pos, 0))
                del us[pos]
            ## Or because we did not move
            elif us.get(old_pos, 0) > 0:
                new_tracking[p] = (old_pos, us.get(old_pos, 0))
                del us[old_pos]
            else:
                ## Nothing found, kill it
                self.parties.remove(p)
        # Then add remaining number to groups
        for pos in us.keys(): # For each position
            new_nb = us[pos]
            if new_nb > 0:
                # Find group at pos
                for p in new_tracking.keys():
                    if new_tracking[p][0] == pos:
                        new_tracking[p] = (pos, new_tracking[p][1] + new_nb)
                        us[pos] = 0
                        break
        if sum(us.values()) != 0:
            print("Error decteting new group position")
            print("Tracking before: ")
            pprint(self.tracking)
            print("Us: " + str(us))
            print("Original: " + str(wh.get_us()))
            print("Tracking after: ")
            pprint(new_tracking)
            if DEBUG:
                raise RuntimeError("Error detecting group positions")

        self.tracking = dict(new_tracking)

    def _select_best_moves(self, parties, wanted_moves, world):
        "For each party, decided the move. wanted_moves is a dict party <-> moves. moves is a list of (score, move, groups) or parent if want to regroup. Returns new parties and moves"
        # Voodoo magic
        # Inputs: parties: list of party; wanted_moves: dict party <-> moves moves is list of (score, move, groups)
        # Ouput: (new_parties, party_moves). party_moves is party <-> (start, nb, stop)
        final_moves = {} # Final moves to be made. Party <-> move
        fulfilled_obj = [] # Already taken objectives
        temp_best = {} # Temporary best moves. Party <-> (move, groups)
        new_parties = []
        remove_parties = [] # Parties to remove at the end
        regroup_parties = [] # Parties that wants to regroup
        # First select best goal for each party
        for party in parties:
            score_moves = wanted_moves[party]
            if type(score_moves) != list: # We want to regroup with parent
                print_main(str(party) + " regrouping...")
                temp_best[party] = score_moves
            else: # score_moves is a list of (score, move, groups) with move = (pos, nb, goal) 
                found = False
                for ask in score_moves:
                    if not ask[1][2] in fulfilled_obj: # The objective was not already fulfilled, do it!
                        temp_best[party] = (ask[1], ask[2]) # (move, groups) with move = (pos, nb, goal) 
                        fulfilled_obj.append(ask[1][2])
                        found = True
                        break
                if not found: # Not found, regroup
                    temp_best[party] = party.parent
    
        # Then create parties
        for party in parties:
            action = temp_best[party] # Action is either parent is want regrouping or tuple of (move, groups) with move = (pos, nb, goal) 
            if type(action) == tuple: # Tuple, create parties
                current_pos = action[0][0]
                main_nb = action[0][1]
                groups = action[1]
                sum = 0 # To delete from main party
                if groups == None: # No group to create, turn it into an empty list list
                    groups = []
                for group in groups: # Group is (nb, pylight party)
                    new_party = group[1]
                    print_main("[PARTY] Creating..." + str(new_party))
                    if DEBUG:
                        assert isinstance(new_party, PylightParty)
                    moves = new_party.select_moves(current_pos, group[0], world, self.fast_minmax_level, self.fast_minmax_level)
                    if type(moves) == list:
                        sum += group[0]
                        goal = world.find_path(current_pos, moves[0][1][2], moves[0][1][1])
                        new_parties.append(new_party)
                        print_party("New party (" + str(group[0]) + ") Going to " + str(moves[0][1][2]) + " throught " + str(goal))
                        final_moves[new_party] = (current_pos, group[0], goal)
                
                main_goal = world.find_path(current_pos, action[0][2], main_nb)
                if DEBUG:
                    assert main_nb - sum > 0
                print_party("(" + str(main_nb - sum) + ") Going to " + str(action[0][2]) + " throught " + str(main_goal))
                final_moves[party] = (current_pos, main_nb - sum, main_goal)
            else: # Regroup with parent
                regroup_parties.append(party)
                
        # Check master wants to regroup
        temp_master = None
        for party in parties:
            if party.parent == None: # This is master
                if (len(parties) + len(new_parties) - len(remove_parties)) > 1: # We still need regrouping
                    if wanted_moves[party][0][0] == PIRATIONAL_CONST: # Score is PIRATIONAL_CONST
                        print_main("Master waiting for children!")
                        all_moves = final_moves.values()
                        goals = [ move[0] for move in all_moves ] # Take goal
                        master_goal = self._barycenter(goals)
                        master_goal = world.find_path(final_moves[party][0], master_goal)
                        temp_master = final_moves[party]
                        temp_master_party = party
                        final_moves[party] = (final_moves[party][0], final_moves[party][1], master_goal)
                        
        # Now regroup orphan parties
        for party in regroup_parties:
            sub_pos = self.tracking[party][0]
            sub_nb = self.tracking[party][1]
            if final_moves.has_key(party.parent):
                parent = party.parent
            else: # Orphan, go to master
                #if DEBUG and party.parent in parties:
                #    raise RuntimeError("No moves ?")
                parent = parties[0]
                
            if parent == party: # Master is dead :'(, promote to master
                print_main("Master seems dead, promoting!")
                party.parent = None
                party.grouping = False
                # And do a quick search
                pos = self.tracking[party][0]
                nb = self.tracking[party][1]
                move = party.select_moves(pos, nb, world, self.fast_minmax_level, self.fast_minmax_level)[0][1]
                goal = world.find_path(move[0], move[2], move[1])
                final_moves[party] = (move[0], move[1], goal)
            else:
                parent_pos = final_moves[parent][0]
                if sub_pos == parent_pos: # They already are on the same cell, group them
                    temp = final_moves[parent]
                    temp = (temp[0], temp[1] + sub_nb, temp[2])
                    final_moves[parent] = temp
                    remove_parties.append(party)
                else: # Go to parent goal
                    parent_goal = final_moves[parent][2]
                    goal = world.find_path(sub_pos, parent_goal, sub_nb)
                    final_moves[party] = (sub_pos, sub_nb, goal)

        # Check again
        still_dont_move = False
        if (len(parties) + len(new_parties) - len(remove_parties)) > 1: # We still need regrouping
            still_dont_move = True
        if not still_dont_move and temp_master:
            temp_master = (temp_master[0], final_moves[temp_master_party][1], temp_master[2])
            final_moves[temp_master_party] = temp_master
        
        return (new_parties, remove_parties, final_moves)
            
    def _barycenter(self, goals):
        "Compute baryenter of goals"
        xs = 0
        ys = 0
        for g in goals:
            xs += g[0]
            ys += g[1]
        return (int(round(xs/len(goals))), int(round(ys/len(goals))))

    def _update_minmax_levels(self, timing):
        print_perf("Update minmax levels with timing %f seconds" % timing)
        if timing < 0.2 and not self.blocked:  # we're runnning fast: increase minmax level
            if self.minmax_level < 20: # Max is 20
                self.minmax_level += 1
        elif timing > 1.7:  # we're running slowly: decrease minmax level
            self.blocked = True
            if self.minmax_level > INITIAL_MINMAX_LEVEL:
                self.minmax_level -= 1
            else:
                self.fast_minmax_level = 1
        print_perf("New minmax level " + str(self.minmax_level) + " and fast " + str(self.fast_minmax_level))

    def callback(self, world):
        "Play best objective"
        # Start clock
        start_time = time.time()

        # Update tracking with the new map
        self._update_tracking(world)
        
        # For each parties, ask its moves
        parties_moves = {}
        for party in self.parties:
            pos = self.tracking[party][0]
            nb = self.tracking[party][1]
            moves = party.select_moves(pos, nb, world, self.minmax_level, self.fast_minmax_level)
            parties_moves[party] = moves

        # Now select best moves
        new_parties, remove_parties, moves = self._select_best_moves(self.parties, parties_moves, world)
        if new_parties or remove_parties:
            self.parties += new_parties
            for p in remove_parties:
                self.parties.remove(p)
        end_moves = [] # To send to callback
        for move in moves.values(): # Move is (old_pos, nb, new_pos)
            end_moves.append((move[0][0], move[0][1], move[1], move[2][0], move[2][1]))

        # Update tracking with moves
        self.tracking = dict(moves)

        removed_moves = self.c.move(end_moves)
        removed_moves = [ ((m[0], m[1]), (m[3], m[4])) for m in removed_moves ] # Set as (old_pos, new_pos)
        # For removed moves, put new_pos = old_pos
        for p in self.tracking.keys():
            temp = self.tracking[p]
            if (temp[0], temp[2]) in removed_moves:
                temp = (temp[0], temp[1], temp[0])
                self.tracking[p] = temp

        # Update depth of min-maxes depending on the timing
        self._update_minmax_levels(time.time() - start_time)

###################
### DEFINITIONS ###
###################

##################
###  __MAIN__  ###
##################

if __name__ == "__main__":
    print_main("> Welcome to " + str(prog_name) + " (r" + str(version) + ")")
    print_main("> By Viq (under CC BY-SA 3.0 license)")
    print_main("> Loading program ...")
    if len(sys.argv) < 3:
        print("Usage: " + str(sys.argv[0]) + " <ip> <port>")
        IP = "127.0.0.1"
        PORT = 5555
    else:
        IP = sys.argv[1]
        PORT = int(sys.argv[2])
        
    with client_api.ClientAPI(server=IP, port=PORT) as client:
        client.mainloop(PylightAI)
