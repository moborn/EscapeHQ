#This code is used for the final exit sounds. when the final puzzle is 
# finished a get request is sent to houdini to play the exit sound. 
# Written by Caleb Parker-Lee - 29/08/2022


from time import sleep
from http.server import SimpleHTTPRequestHandler, HTTPServer
from urllib.error import HTTPError, URLError
from urllib.request import urlopen
from gpiozero import Button


import logging
logging.basicConfig(filename='exitSoundLog.txt', level = logging.DEBUG, format='%(asctime)s %(message)s')
#logging call: 'logging.debug('message')'

#Taking pin 26 as input
button = Button(26)

x = 0 #x is the number of time the final puzzle was solved

#Function for when the final puzzle is finished
def exit():
    global x
    x = x + 1
    print("pressed")
    urlopen("http://192.168.178.74:15001/stop") ##blizz 1
    # urlopen("http://192.168.178.74:15002/stop") ##blizz 2
    logging.debug('Final puzzle was sovled: ' + str(x))

#############################################
################ MAIN LOOP ##################
############################################
while True:
    
    #button.when_released = exit
    button.when_pressed = exit
    sleep(10)
