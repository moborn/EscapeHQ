import socket
from http.server import HTTPServer, BaseHTTPRequestHandler
import logging
import RPi.GPIO as GPIO
import pygame
import subprocess as sp

pygame.mixer.init()
logging.basicConfig(filename='relayLog.txt', level = logging.DEBUG, format='%(asctime)s %(message)s')

#Logging
def log(message):
    if(DEBUG):
        logging.debug(message)

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
DEBUG = False
PORT = 15007

class Relay:
    def __init__(self, gpio, state, name):
        self.name = name

class Relay:
    def __init__(self, gpio, state, name):
        self.name = name
        self.gpio = gpio
        self.state = state 
        self.init_state = state 
        self.changed = False 
        GPIO.setup(self.gpio, GPIO.OUT) 
        GPIO.output(self.gpio, self.state) 
 
    def getName(self): 
        return self.name

    def getState(self):
        return self.state

    def setState(self, state):
        self.changed = self.state != state
        self.state = state
        GPIO.output(self.gpio, self.state)

    def reset(self):
        self.changed = self.state != self.init_state 
        self.state = self.init_state 
        GPIO.output(self.gpio, self.state) 
p38 = Relay(20, 0, "relay_7")  # Office Access (Keypad)

relays = [p38]

def playSound():
    pygame.mixer.music.set_volume(1)
    pygame.mixer.music.load("/home/pi/Scripts/jailalarm.mp3")
    pygame.mixer.music.play()
def stopSound():
    pygame.mixer.music.stop()

def switchRelayState(name): 
    for relay in relays: 
        if relay.getName() == name: 
            state = relay.getState() != 1 
            relay.setState(state) 
            print("Setting relay {} to {}".format(name, state)) 
            log("{} is {}".format(relay.getName(), relay.getState())) 
            playSound()
            break
extProc = sp.Popen(['python','keypad.py'], cwd='/home/pi/Scripts') 

class requestHandler(BaseHTTPRequestHandler):
    ##THIS IS THE CORE FUNCTION LISTENING TO HOUDINI
    ## listening for http requests, processing the string and seperating for the relay name
    ## switchRelayState function further parses to get relay number
    def do_GET(self):
        try:
            self.send_response(200) 
            self.send_header('content-type', 'text/html')
            self.end_headers()
            request = self.path[1:]
            if 'relay' in request : switchRelayState(request)
            elif 'stop-alarm' in request:
                stopSound()
        except IOError:
            self.send_error(500, "Server Error")

def getLocalIP():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    ip = s.getsockname()[0]
    s.close()
    return ip



def main():
    print("Attempting Connection")
    print(getLocalIP())
    server_address = (getLocalIP(), PORT)
    server = HTTPServer(server_address, requestHandler)
    print("Server running")
    server.serve_forever() 

if __name__ == '__main__':
    main()