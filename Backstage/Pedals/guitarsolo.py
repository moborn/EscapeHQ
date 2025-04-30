import time  #need this for sleep
import pigpio
pi = pigpio.pi() #initialises pigpio
import urllib.request
from threading import Thread  #threading does multiple things at once
import pygame
import sys
pygame.mixer.init()

complete = False

def playSound():
    pygame.mixer.music.load("/home/pi/Scripts/Feels.mp3")
    pygame.mixer.music.play()

RedPin = 17
GreenPin = 24
BluePin = 22
pi.set_pull_up_down(26, pigpio.PUD_DOWN)  #puzzle switch - so this will be on pedal 4. 
pi.set_pull_up_down(13, pigpio.PUD_DOWN)   #override switch
pi.set_pull_up_down(19, pigpio.PUD_DOWN)	#pedal 1
pi.set_pull_up_down(21, pigpio.PUD_DOWN)	#pedal 2
pi.set_pull_up_down(23, pigpio.PUD_DOWN)	#pedal 3
pi.set_mode(16, pigpio.OUTPUT)   #smoke machine relay pin
pi.set_glitch_filter(13, 100)

def callbackOverideSwitch(gpio, level, tick):  #callback listening for a high signal on the puzzle switch
    global complete
    if pi.read(13) == 1:
        print("Override switch triggered")
        time.sleep(0.2)
        complete = True

        try:
            print("start houdini thread")   #tells houdini to stop timer
            houdinithread().start()
        except:
            pass

        print("PuzzleSwitch triggered")
        time.sleep(0.2)
            
        ledsOff()                       #leds turn off
        playSound()                     #audio plays
        time.sleep(0.2)
        flashing()

        print("Game complete")
    
def callbackPuzzleSwitch(gpio, level, tick):     #callback listening for a high signal on puzzle circuit
    global complete
    if pi.read(19) == 1 and complete == False:
        time.sleep(0.1)
        if pi.read(26) == 1 and pi.read(19) == 1 and pi.read(21) == 1 and pi.read(23) == 1: #if all pedals are pressed
            complete = True
            
            try:
                print("start houdini thread")   #tells houdini to stop timer
                houdinithread().start()
            except:
                pass

            print("PuzzleSwitch triggered")
            time.sleep(0.2)
            
            ledsOff()                       #leds turn off
            playSound()                     #audio plays
            time.sleep(0.2)
            flashing()
            
            print("Game complete")

cb1 = pi.callback(13, pigpio.EITHER_EDGE, callbackOverideSwitch)
cb2 = pi.callback(26, pigpio.EITHER_EDGE, callbackPuzzleSwitch)
cb3 = pi.callback(19, pigpio.EITHER_EDGE, callbackPuzzleSwitch)
cb4 = pi.callback(21, pigpio.EITHER_EDGE, callbackPuzzleSwitch)
cb5 = pi.callback(23, pigpio.EITHER_EDGE, callbackPuzzleSwitch)

class houdinithread(Thread):                         #set up as a thread incase houdini isn't on it will still continue with the rest of the script
   def run(self):
    url = "http://192.168.178.117:15005/stop"
    try:
        response = urllib.request.urlopen(url).read()
    except:
        print("no connection")

def flashing():
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

try:
    while(True):
        if complete == False:
            ledsGreen()
except:
    print("Cleaning up")
    ledsOff()
    pi.stop()
    sys.exit()