import time  #need this for sleep
import pigpio
pi = pigpio.pi() #initialises pigpio
import urllib.request
import socket
from http.server import HTTPServer, BaseHTTPRequestHandler
from threading import Thread  #threading does multiple things at once
import pygame
import sys
pygame.mixer.init()

complete = False
PORT = 15005

def playSound():
    pygame.mixer.music.load("/home/pi/Scripts/Feels.mp3")
    pygame.mixer.music.play()

RedPin = 17
GreenPin = 24
BluePin = 22
pi.set_pull_up_down(26, pigpio.PUD_DOWN)  #puzzle switch
pi.set_pull_up_down(13, pigpio.PUD_DOWN)   #override switch
pi.set_pull_up_down(19, pigpio.PUD_DOWN)	#double check on pedal 1
pi.set_mode(16, pigpio.OUTPUT)   #smoke machine relay pin
pi.set_glitch_filter(13, 100)

def callbackOverideSwitch(gpio, level, tick):  #callback listening for a high signal on the puzzle switch
    time.sleep(0.5)
    if pi.read(13) == 1:
        
        print("Override switch triggered")
        time.sleep(0.2)
            
        ledsOff()                       #leds turn off
        playSound()                     #audio plays
        time.sleep(0.4)
        flashingthread2().start()
        print("flashing2 starts")
        time.sleep(12)

        print("start houdini thread")   #tells houdini to stop timer
        houdinithread().start()
            
        print("Game complete")
        complete = True
    
def callbackPuzzleSwitch(gpio, level, tick):     #callback listening for a high signal on puzzle circuit
    global complete
    print("19",pi.read(19))
    print("26", pi.read(26))
    if pi.read(19) == 1 and complete == False:
        time.sleep(0.1)
        if pi.read(26) == 1 and pi.read(19) == 1:
            complete = True
            print("PuzzleSwitch triggered")
            time.sleep(0.2)
            
            ledsOff()                       #leds turn off
            playSound()                     #audio plays
            time.sleep(0.2)
            flashingthread2().start()
            print("flashing2 starts")
            time.sleep(12)

            print("start houdini thread")   #tells houdini to stop timer
            houdinithread().start()
            
            print("Game complete")

cb1 = pi.callback(13, pigpio.EITHER_EDGE, callbackOverideSwitch)
cb2 = pi.callback(26, pigpio.EITHER_EDGE, callbackPuzzleSwitch)

class houdinithread(Thread):                         #set up as a thread incase houdini isn't on it will still continue with the rest of the script
   def run(self):
    url = "http://192.168.178.117:15005/stop"
    response = urllib.request.urlopen(url).read()

class flashingthread2(Thread):
    def run(self):
            y=1
            while y<=4:
                ledsOff()
                time.sleep(0.63)
            
                ledsRed()
                time.sleep(1)
                y+=1
                
            while y==5:
                ledsRed()
                time.sleep(0.4)
                y+=1
                
            while y>5 and y<=47:
                ledsOff()
                time.sleep(0.1)

                ledsRed()
                time.sleep(0.1)
                y+=1
            else:
                ledsOff()
                time.sleep(0.3)
                ledsRed()
                
def ledsBlue():
    pi.set_PWM_dutycycle(RedPin, 0)
    pi.set_PWM_dutycycle(GreenPin, 0)
    pi.set_PWM_dutycycle(BluePin, 255)
    
def ledsOff():
    pi.set_PWM_dutycycle(RedPin, 0)
    pi.set_PWM_dutycycle(GreenPin, 0)
    pi.set_PWM_dutycycle(BluePin, 0)
    
def ledsRed():
    pi.set_PWM_dutycycle(RedPin, 255)
    pi.set_PWM_dutycycle(GreenPin, 0)
    pi.set_PWM_dutycycle(BluePin, 0)
 
def ledsGreen():
    pi.set_PWM_dutycycle(RedPin, 0)
    pi.set_PWM_dutycycle(GreenPin,255)
    pi.set_PWM_dutycycle(BluePin, 0)

class requestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        global complete
        try:
            self.send_response(200)
            self.send_header('content-type', 'text/html')
            self.end_headers()
            request = self.path[1:]
            print("Received request: ", request)
            if (request == "reset"):
                complete = False
        except IOError:
            self.send_error(500, "Server Error")

class websocketThread(Thread):
    def run(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()

        print("Attempting Connection")
        print(ip)
        server_address = (ip, PORT)
        server = HTTPServer(server_address, requestHandler)
        print("Server running")
        server.serve_forever()

while True:
    try:
        ledsRed()
        time.sleep(1)
    except:
        ledsOff()
        pi.stop()
        sys.exit()

try:
    websocket = websocketThread()
    websocket.setDaemon(True)
    websocket.start()

    while(True):
        if complete == False:
            ledsGreen()
        time.sleep(0.1)
except:
    print("Cleaning up")
    ledsOff()
    pi.stop()
    sys.exit()
