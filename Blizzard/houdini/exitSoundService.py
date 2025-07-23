#This code is used for the final exit sounds. when the final puzzle is 
# finished a get request is sent to houdini to play the exit sound. 
# Written by Caleb Parker-Lee - 29/08/2022


from time import sleep
from urllib.request import urlopen
from http.server import HTTPServer, BaseHTTPRequestHandler
from gpiozero import Button
import logging
logging.basicConfig(filename='exitSoundLog.txt', level = logging.DEBUG, format='%(asctime)s %(message)s')

#Logging
def log(message):
    if(DEBUG):
        logging.debug(message)

DEBUG = True
ENDGAMEPIN = 26 #GPIO 26
# ENDGAMEURL = "http://192.168.178.74:15002/stop" ##this is blizz 2
ENDGAMEURL = "http://192.168.178.74:15001/stop" ## this is blizz 1
button = Button(ENDGAMEPIN)

#Function for when the final puzzle is finished
def exit():
    urlopen(ENDGAMEURL)
    print("flare trigger")
    log("Final puzzle was sovled")

def main():
            while True:
                button.when_pressed = exit
                sleep(10)

#Main Loop
if __name__ == '__main__':
        main()
