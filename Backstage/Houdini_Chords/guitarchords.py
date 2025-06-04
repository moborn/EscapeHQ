from http.server import BaseHTTPRequestHandler, HTTPServer
from time import sleep                  # may need to "debounce" switches
import threading

from http.server import ThreadingHTTPServer


import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)  # Set's GPIO pins to BCM GPIO numbering

import collections
import urllib.request
import pygame
import subprocess as sp
import socket
pygame.mixer.init()
PORT = 15006

complete = False

# HOUDINI_URL = "http://192.168.178.117:15005"
HOUDINI_URL = "http://192.168.178.74:14999"

sequenceRequired = "05871"  #correct combo  
sequence = collections.deque(maxlen=5)              #variable to store combo 
digitPressed = ()

GPIO.setup(2, GPIO.IN, pull_up_down=GPIO.PUD_UP)#red
GPIO.setup(3, GPIO.IN, pull_up_down=GPIO.PUD_UP)#orange
GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_UP)#yellow

GPIO.setup(14, GPIO.IN, pull_up_down=GPIO.PUD_UP)#grey
GPIO.setup(15, GPIO.IN, pull_up_down=GPIO.PUD_UP)#purple
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)#gold
GPIO.setup(23, GPIO.IN, pull_up_down=GPIO.PUD_UP)#brown

GPIO.setup(25, GPIO.IN, pull_up_down=GPIO.PUD_UP)#blue
GPIO.setup(8, GPIO.IN, pull_up_down=GPIO.PUD_UP)#light blue
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_UP)#green

# Functions to run when each button is pressed.
def Input_1(channel):
    global digitPressed
    global sequence
    digitPressed = 1
    print(digitPressed)
    sequence.append(digitPressed)

def Input_2(channel):
    global digitPressed
    global sequence
    digitPressed = 2
    print(digitPressed)
    sequence.append(digitPressed)

def Input_3(channel):
    global digitPressed
    global sequence
    digitPressed = 3
    print(digitPressed)
    sequence.append(digitPressed)

def Input_4(channel):
    global digitPressed
    global sequence
    digitPressed = 4
    print(digitPressed)
    sequence.append(digitPressed)
    
def Input_5(channel):
    global digitPressed
    global sequence
    digitPressed = 5
    print(digitPressed)
    sequence.append(digitPressed)

def Input_6(channel):
    global digitPressed
    global sequence
    digitPressed = 6
    print(digitPressed)
    sequence.append(digitPressed)
    
def Input_7(channel):
    global digitPressed
    global sequence
    digitPressed = 7
    print(digitPressed)
    sequence.append(digitPressed)

def Input_8(channel):
    global digitPressed
    global sequence
    digitPressed = 8
    print(digitPressed)
    sequence.append(digitPressed)
    
def Input_9(channel):
    global digitPressed
    global sequence
    digitPressed = 9
    print(digitPressed)
    sequence.append(digitPressed)
    
def Input_10(channel):
    global digitPressed
    global sequence
    digitPressed = 0
    print(digitPressed)
    sequence.append(digitPressed)

#05873
# Does a Callback to the appropriate Input function.  Also debounces to prevent clicking the button multiple times a second.
GPIO.add_event_detect(2, GPIO.FALLING, callback=Input_1, bouncetime=10000) # Waiting for Button 1 to be pressed.
GPIO.add_event_detect(3, GPIO.FALLING, callback=Input_2, bouncetime=10000) # Waiting for Button 2 to be pressed.
GPIO.add_event_detect(4, GPIO.FALLING, callback=Input_3, bouncetime=10000) # Waiting for Button 3 to be pressed.
GPIO.add_event_detect(14, GPIO.FALLING, callback=Input_4, bouncetime=10000) # Waiting for Button 1 to be pressed.
GPIO.add_event_detect(15, GPIO.FALLING, callback=Input_5, bouncetime=10000) # Waiting for Button 2 to be pressed.
GPIO.add_event_detect(18, GPIO.FALLING, callback=Input_6, bouncetime=10000) # Waiting for Button 3 to be pressed.
GPIO.add_event_detect(23, GPIO.FALLING, callback=Input_7, bouncetime=10000) # Waiting for Button 1 to be pressed.
GPIO.add_event_detect(25, GPIO.FALLING, callback=Input_8, bouncetime=10000) # Waiting for Button 1 to be pressed.
GPIO.add_event_detect(8, GPIO.FALLING, callback=Input_9, bouncetime=10000) # Waiting for Button 2 to be pressed.
GPIO.add_event_detect(7, GPIO.FALLING, callback=Input_10, bouncetime=10000) # Waiting for Button 3 to be pressed.


def solved():
    global complete
    complete = True
    print("Puzzle complete")
    
    try:
        url = HOUDINI_URL + "/guitarchords2"
        response = urllib.request.urlopen(url).read()
        sequence.append(4)
        print("video1")
    except:
        print("can't connect to houdini")

def mainRoutine():
    global sequence
    global sequenceRequired

    if digitPressed is None:
        return

    sleep(0.10000)
    sequencestring = (''.join(map(str,sequence)))
    if sequencestring == sequenceRequired:
        solved()

class MyHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        global complete
        self.send_response(200)
        self.send_header('content-type', 'text/html')
        self.end_headers()

        if self.path == '/play-video':
            if not complete:
                print("Override received")
                complete = True
        elif self.path == '/reset':
            print("Reset received")
            complete = False

ip = None
while ip is None:
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8",80))
        ip = s.getsockname()[0]
        s.close()
    except:
        print("Waiting for internet")
        sleep(1)

server_address = (ip, PORT)
server = ThreadingHTTPServer(server_address, MyHandler)
server_thread = threading.Thread(target=server.serve_forever)
server_thread.daemon = True
server_thread.start()
print("Attempting Connection")
print(ip)
print("Server running")

while(True):
    if complete:
        solved()
        sleep(40) ## THIS IS A DELAY IN SECONDS!! NOT MILLISECONDS
    else:
        mainRoutine()
