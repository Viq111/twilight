# -*- coding:Utf-8 -*-
# x Package
prog_name = "myprog"
# version:
version = 1
# By Viq - Vianney Tran
# License: Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0) 
# (http://creativecommons.org/licenses/by-sa/3.0/)

##############
### IMPORT ###
##############
import os, time, sys, subprocess

###############
### GLOBALS ###
###############

DEFAULT_AI = "ai_nearest_possible.py"
SERVER_PATH = os.path.join("..", "server", "official", "VampiresVSWerewolvesGameServer.exe")

###############
### CLASSES ###
###############


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
    if len(sys.argv) < 2 or len(sys.argv) >= 4:
        print "Usage: " + sys.argv[0] + " <AI python file1> (<AI python file2>)"
        p1 = p2 = DEFAULT_AI
        print "Defaulting to: " + p1
    elif len(sys.argv) == 2:
        p1 = p2 = sys.argv[1]
    else:
        p1 = sys.argv[1]
        p2 = sys.argv[2]
    print "BATTLE: " + str(p1) + " VS " + str(p2)
    # Launch server
    server = subprocess.Popen(SERVER_PATH, cwd=os.path.dirname(SERVER_PATH))
    time.sleep(3)
    # Launch both AI
    ai1 = subprocess.Popen(["python", p1], stdout=subprocess.PIPE)
    time.sleep(0.4)
    ai2 = subprocess.Popen(["python", p2], stdout=subprocess.PIPE)
    wait = raw_input("Press enter to kill...")
    ai1.terminate()
    ai2.terminate()
    server.terminate()
    ai1.wait()
    ai2.wait()
    server.wait()
    
        
