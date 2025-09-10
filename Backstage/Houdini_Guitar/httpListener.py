from http.server import BaseHTTPRequestHandler, HTTPServer
import RPi.GPIO as gpio
import pygame
import subprocess as sp
import socket
import time
import os
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
def roomaudio(boolean):
    ##add a try incase this fails
    while True:
        try:
            pygame.mixer.music.load("/home/pi/Scripts/theme_2hrs.mp3")
            break
        except Exception as e:
            ##try open file
            try:
                with open(logging_file, 'a') as f:
                    f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - Error loading music: {e}, retrying in 1 second...\n")
            except Exception as file_error:
                print(f"Error opening log file: {file_error}")
            print(f"Error loading music: {e}, retrying in 1 second...")
            # time.sleep(1)
    if boolean == True:
        with open(logging_file, 'a') as f:
                    f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - main theme start\n")
        print("main theme start")
        pygame.mixer.music.set_volume(0.025)
        pygame.mixer.music.play()
    elif boolean == False:
        with open(logging_file, 'a') as f:
                    f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - main theme stop\n")
        print("main theme stop")
        pygame.mixer.music.stop()
def playSound():
    pygame.mixer.music.set_volume(1)
    pygame.mixer.music.load("/home/pi/Scripts/RoomService.mp3")
    pygame.mixer.music.play()
    while pygame.mixer.music.get_busy() == True:
       continue
    roomaudio(True)

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
                with open(logging_file, 'a') as f:
                    f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - room service played\n")
                roomaudio(False)
                playSound()
        elif self.path == '/reset':
            print("reset called")
            complete=False
            resetPuzzles()
            roomaudio(True)
        elif self.path == '/pop-guitar':
            print("overrride called")
            roomaudio(False)
            gpio.output(13, gpio.HIGH)
            with open(logging_file, 'a') as f:
                f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')} - guitar popped\n")
        elif self.path =='/stop-theme':
            print("houdini exit trigger")
            roomaudio(False)
           # gpio.output(13, gpio.HIGH)


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

logging_file = "/home/pi/Scripts/audiolog.txt"
logging_file = os.path.join(os.path.expanduser("~"), logging_file)

if os.path.exists(logging_file):
    with open(logging_file, 'r') as f:
        lines = f.readlines()
    if len(lines) > 500:
         os.remove(logging_file)
         with open(logging_file, 'w') as f:
             f.write("LOGGING FILE\n")
else:   
    with open(logging_file, 'w') as f:
        f.write("LOGGING FILE\n")
roomaudio(True)
try :
    server.serve_forever()
except:
    sp.Popen.terminate(extProc) # closes the process
    time.sleep(3)
