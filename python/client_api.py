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
import os, time, socket, threading

###############
### GLOBALS ###
###############

DEFAULT_SERVER = "127.0.0.1"
DEFAULT_PORT = 5555

###############
### CLASSES ###
###############

class World():
    "Create an object to keep the world"
    def __init__(self, n, m):
        "Create a world NxM: M lines N columns"
        self.world = []
        self.im_vampire = True
        for i in range(m):
            line = []
            for j in range(n):
                cell = {"us" : 0, "ennemy" : 0, "human" : 0}
                line.append(cell)
            self.world.append(line)
            
    def get_world_as_matrix(self):
        "Return the world as a matrix"
        return self.world

    def get_size(self):
        "Return the size"
        return (len(self.world), len(self.world[0]))

    def get_cell(self, x, y):
        "Return the content of a cell"
        return self.world[x][y]

    def init_map(self, my_pos, positions):
        "Initialize the map"
        # First know if we are vampire or wolf
        print my_pos, positions
        for p in positions:
            if my_pos == (p[0], p[1]):
                if p[3] != 0:
                    print "You are a vampire"
                    self.im_vampire = True
                elif p[4] != 0:
                    print "You are a wolf"
                    self.im_vampire = False
                else:
                    raise RuntimeError("Cannot detect your race :'( " + str(my_pos) + " and " + str(positions))
        # Then update map
        for p in positions:
            self.update(p[0], p[1], p[2], p[3], p[4])

    def update(self, x, y, human, vamp, wolf):
        "Update the number of players"
        if self.im_vampire:
            self.world[x][y] = {"us" : vamp, "ennemy" : wolf, "human" : human}
        else:
            self.world[x][y] = {"us" : wolf, "ennemy" : vamp, "human" : human}

    def find_path(self, start, stop):
        "Find a path from start to stop"
        # To Do: A* algorithm !
        x = stop[0] - start[0]
        y = stop[1] - start[1]
        if x < 0: x = -1
        if x > 0: x = +1
        if y < 0: y = -1
        if y > 0: y = +1
        return (start[0]+x, start[1]+y)                
        
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
        print str(self.__name) + ", your time to play!"
    
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
        data = self.s.recv(5)
        if data[0:3] != "SET":
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
                    print "Unknown command: " + str([cmd])
            except socket.timeout:
                pass
        self.s.close()

    def get_map(self):
        "Get the Map Obejct"
        return self.world

    def move(self, moves):
        "Move your player move is (initial_x, initial_y, nb, dest_x, dest_y)"
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
    
