from http.server import BaseHTTPRequestHandler, HTTPServer

import RPi.GPIO as gpio
import pygame
import subprocess as sp
import socket
import time
pygame.mixer.init()
gpio.setmode(gpio.BCM)
gpio.setup(13, gpio.OUT)
gpio.output(13, gpio.LOW)

complete = False
PORT = 15005

extProc = sp.Popen(['python','guitarsolo.py'], cwd='/home/pi/Scripts') # runs myPyScript.py

def resetPuzzles():
    global extProc
    gpio.output(13,gpio.LOW)
    sp.Popen.terminate(extProc)
    time.sleep(3)
    extProc = sp.Popen(['python','guitarsolo.py'], cwd='/home/pi/Scripts') # runs myPyScript.py

def playSound():
    pygame.mixer.music.load("/home/pi/Scripts/RoomService.mp3")
    pygame.mixer.music.play()

class MyHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('content-type', 'text/html')
        self.end_headers()

        global complete
        if self.path == '/room-service':
            if not complete:
                complete=True
                print("Playing sound")
                playSound()
        elif self.path == '/reset':
            print("reset called")
            complete=False
            resetPuzzles()
        elif self.path == '/pop-guitar':
            print("overrride called")
            gpio.output(13, gpio.HIGH)

ip = None
while ip is None:
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8",80))
        ip = s.getsockname()[0]
        s.close()
    except:
        print("Waiting for internet")
        time.sleep(1)

print("Attempting Connection")
print(ip)

server_address = (ip, PORT)
server = HTTPServer(server_address, MyHandler)

print("Server running")


try :
    server.serve_forever()
except:
    sp.Popen.terminate(extProc) # closes the process
    time.sleep(3)