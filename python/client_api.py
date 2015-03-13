# -*- coding:Utf-8 -*-
# Twilight Package
prog_name = "Twilight - Client API"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time, socket, threading, sys
import heapq # For A* search
import termcolor

###############
### GLOBALS ###
###############

DEFAULT_SERVER = "127.0.0.1"
DEFAULT_PORT = 5555

def color_print(*args):
    "Print thing with client_api color"
    COLOR = "yellow"
    s = "[API] "
    for a in args:
        s += str(a)
    termcolor.cprint(s, COLOR)

###############
### CLASSES ###
###############

# PriorityQueue for A* algorithm
# From http://www.redblobgames.com/pathfinding/a-star/implementation.html#sec-1-4

class PriorityQueue:
    def __init__(self):
        self.elements = []
    
    def empty(self):
        return len(self.elements) == 0
    
    def put(self, item, priority):
        heapq.heappush(self.elements, (priority, item))
    
    def get(self):
        return heapq.heappop(self.elements)[1]

class World():
    "Create an object to keep the world"
    def __init__(self, n, m):
        "Create a world NxM: M lines N columns"
        self.world = []
        self.im_vampire = True
        self.inited = False
        self.init_lock = threading.Condition()
        for i in range(m):
            line = []
            for j in range(n):
                cell = {"us" : 0, "ennemy" : 0, "human" : 0}
                line.append(cell)
            self.world.append(line)

    def __wait_init(self):
        "If the world has not been inited, wiat for it to be inited"
        with self.init_lock:
            if self.inited:
                return True
            else:
                color_print("> Waiting for init...")
                self.init_lock.wait()
            color_print("> Map inited!") 
            return True

    # Useful for clients

    def get_starting_position(self):
        "Return (x, y, number) of the starting position"
        self.__wait_init()
        for col in range(len(self.world)):
            for line in range(len(self.world[0])):
                if self.get_cell(col, line)["us"] != 0:
                    return (col, line)

    def get_size(self):
        "Return the size"
        self.__wait_init()
        return (len(self.world), len(self.world[0]))

    def get_cell(self, x, y):
        "Return the content of a cell"
        self.__wait_init()
        return self.world[x][y]

    def get_world_as_matrix(self):
        "Return the world as a matrix"
        return self.world

    # Useful for network (client_api)

    def init_map(self, my_pos, positions):
        "Initialize the map"
        with self.init_lock:
            # First know if we are vampire or wolf
            for p in positions:
                if my_pos == (p[0], p[1]):
                    if p[3] != 0:
                        color_print("You are a vampire")
                        self.im_vampire = True
                    elif p[4] != 0:
                        color_print("You are a wolf")
                        self.im_vampire = False
                    else:
                        raise RuntimeError("Cannot detect your race :'( " + str(my_pos) + " and " + str(positions))
            # Then update map
            for p in positions:
                self.update(p[0], p[1], p[2], p[3], p[4])
            # Map inited
            self.inited = True
            self.init_lock.notify()

    def update(self, x, y, human, vamp, wolf):
        "Update the number of players"
        if self.im_vampire:
            self.world[x][y] = {"us" : vamp, "ennemy" : wolf, "human" : human}
        else:
            self.world[x][y] = {"us" : wolf, "ennemy" : vamp, "human" : human}

    def _get_neighboors(self, pos, goal = None):
        "Get possible neigboor moves, set the goal if you want to consider it as free"
        neightboors = []
        for i in range(-1, 2):
            for j in range(-1, 2):
                x = pos[0] + i
                y = pos[1] + j
                if (i != 0 or j != 0) and x >= 0 and y >= 0 and x < len(self.world) and y < len(self.world[0]): # Cell inside the grid
                    cell = self.get_cell(x, y)
                    if cell["ennemy"] == 0 and cell["human"] == 0:
                        neightboors.append((x, y))
                    elif goal != None:
                        if x == goal[0] and y == goal[1]:
                            neightboors.append((x, y))
        return neightboors

    def _get_flight_distance(self, start, stop):
        "Get a flight distance from start to stop"
        return max(abs(stop[0] - start[0]), abs(stop[1] - start[1]))

    def _a_star(self, start, stop):
        "Do A* algorithm from start to stop, return the path"
        # A* from http://www.redblobgames.com/pathfinding/a-star/implementation.html#sec-1-4
        if start == stop: # No path, we already are there
            return []
        frontier = PriorityQueue()
        frontier.put(start, 0)
        came_from = {start : None}
        cost_so_far = {start : 0}
        while not frontier.empty():
            current = frontier.get()
            if current == stop:
                break
            for next in self._get_neighboors(current, stop):
                new_cost = cost_so_far[current] + 1
                if next not in cost_so_far or new_cost < cost_so_far[next]:
                    cost_so_far[next] = new_cost
                    priority = new_cost + self._get_flight_distance(stop, next)
                    frontier.put(next, priority)
                    came_from[next] = current
        # Check if we found a path
        if not came_from.has_key(stop): # Cannot find a path
            return []
        else:
            path = [stop]
            while came_from[path[0]] != start: # We are not at the start
                path.insert(0, came_from[path[0]])
            return path  
            
    def find_path(self, start, stop):
        "Find a free path (no obstacle like an ennemy or human) from start to stop with A*"
        self.__wait_init()
        res = self._a_star(start, stop)
        if len(res) == 0:
            return start
        else:
            return res[0]

    def find_path_time(self, start, stop):
        "Find the time to move from start to stop"
        self.__wait_init()
        res = self._a_star(start, stop)
        if start == stop:
            return 0
        if len(res) == 0:
            return 99999999999 # There is no path
        else:
            return len(res)
        
    def __repr__(self):
        "Show the map"
        ret = []
        for l in range(len(self.world[0])):
            line = []
            for col in range(len(self.world)):
                c = self.world[col][l]
                if c["us"] != 0:
                    line.append("M" + str(c["us"]))
                elif c["ennemy"] != 0:
                    line.append("E" + str(c["ennemy"]))
                elif c["human"] != 0:
                    line.append("H" + str(c["human"]))
                else:
                    line.append("  ")
            line = "|".join(line)
            ret.append(line)
        return "\n".join(ret)

class ClientAPI(threading.Thread):
    "Create a simple connection to the vampire/wolves server"
    def __default_callback(self, world):
        "This is called when it's our turn to play"
        print(str(self.__name) + ", your time to play!")
    
    def __init__(self, server = DEFAULT_SERVER, port = DEFAULT_PORT, turn_callback = None):
        "Connect to the server"
        self.ip = server
        self.port = port
        self.running = False
        self.__name = "Unknown"
        if turn_callback:
            self.callback = turn_callback
        else:
            self.callback = self.__default_callback

        threading.Thread.__init__(self)

    def connect(self, my_name):
        "Connect to the server"
        if self.callback == self.__default_callback:
            sys.stderr.write("[WARNING] Client_api.connect() called before set_callback, you AI may not be called for the first turn")
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((self.ip, self.port))
        self.__name = my_name
        self._init_com()
        self.s.settimeout(2)
        # Start the callback to handle map update
        self.start()

    def _init_com(self):
        "Init communication"
        self.s.send("NME" + chr(len(self.__name)) + self.__name)
        # First get the map size
        try:
            data = self.s.recv(5)
        except socket.error:
            color_print("End of game (Socket Error)")
            self.close()
            return
        if data[0:3] != "SET":
            if data[0:3] == "BYE":
                color_print("End of game (BYE)")
                self.close()
                return
            raise RuntimeError("First command was not SET: " + str(data))
        self.world = World(ord(data[3]), ord(data[4]))

    def close(self):
        "Close the client"
        self.running = False

    def set_callback(self, callback):
        "Set a new callback"
        self.callback = callback
        
    def __enter__(self):
        "RAII Return the screen with the keyword with"
        return self
    
    def __exit__(self, type, value, traceback):
        "Return the console to normal"
        self.close()

    def run(self):
        "Handle map updates"
        self.running = True
        temp_starting_pos = (0,0)
        while self.running:
            try:
                cmd = self.s.recv(3) # Get command
                if cmd == "HUM": # This is useless
                    lenght = ord(self.s.recv(1))*2
                    # Wait for everything to pass
                    self.s.recv(lenght)
                elif cmd == "HME": # Starting position
                    data = self.s.recv(2)
                    temp_starting_pos = (ord(data[0]), ord(data[1]))
                elif cmd == "MAP": # Init map
                    length = ord(self.s.recv(1))*5
                    data = self.s.recv(length)
                    # Change str to int
                    data = map(lambda a: ord(a), data)
                    # Group by 5
                    pos = [data[i*5:(i*5)+5] for i in range(len(data)/5)]
                    # Update initial MAP
                    self.world.init_map(temp_starting_pos, pos)
                elif cmd == "UPD": # Update
                    length = ord(self.s.recv(1))*5
                    if length > 0:
                        data = self.s.recv(length)
                        # Change str to int
                        data = map(lambda a: ord(a), data)
                        # Group by 5
                        pos = [data[(i*5):(i*5)+5] for i in range(len(data)/5)]
                        for p in pos:
                            self.world.update(p[0], p[1], p[2], p[3], p[4])
                    # Call callback
                    self.callback(self.world)
                elif cmd == "END":
                    # Restart game
                    self._init_com()
                elif cmd == "BYE":
                    # End of server
                    self.close()
                else:
                    color_print("/!\\Unknown command: " + str([cmd]))
            except socket.timeout:
                pass
        self.s.close()

    def get_map(self):
        "Get the Map Obejct"
        return self.world

    def move(self, moves, autoremove_forbiden_moves = True):
        "Send multiple moves command, moves is a list of (initial_x, initial_y, nb, dest_x, dest_y). Autoremove removes fordien moves (same source and destination cell)."
        if autoremove_forbiden_moves:
            # Remove forbiden moves
            while 1:
                sources = {}
                dest = {}
                for move in moves:
                    sources[(move[0], move[1])] = move
                    dest[(move[3], move[4])] = move
                removed = False
                for d in dest.keys():
                    if sources.has_key(d): # If we find a forbiden move, remove it
                        removed = True
                        color_print("Forbiden move: " + str(move))
                        moves.remove(dest[d])
                        break
                if not removed: # We did not remove anything, there is no more something wrong
                    break
            
        cmd = "MOV"
        cmd += chr(len(moves))
        for move in moves:
            for i in range(len(move)):
                cmd += chr(move[i])
        self.s.send(cmd)
    
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
    import time
    c = ClientAPI()
    c.connect("Viq")
    print "Quitting in 4s..."
    time.sleep(2)
    print "MOVING"
    c.move((5,4,2,4,4))
    time.sleep(2)
    print "Closing..."
    print c.get_map()
    c.close()
    
