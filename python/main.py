import os, time, traceback, copy
import termcolor
import client_api
import minmax
from ai_pylight import *

if __name__ == "__main__":
    print_main("> By Viq (under CC BY-SA 3.0 license)")
    print_main("> Loading program ...")
    with client_api.ClientAPI() as client:
        client.mainloop(PylightAI)