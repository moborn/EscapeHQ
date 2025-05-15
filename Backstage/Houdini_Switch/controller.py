import sys
import time  #need this for sleep
import traceback
import pigpio
import socket
import requests
from http.server import HTTPServer, BaseHTTPRequestHandler

pi = pigpio.pi() #initialises pigpio

PORT = 15005

# Misc pins
ResetPin = 19
FilterPin = 2

# Signal pins to switch the relays off
FrontDoorPin = 10
ClosetDoorPin = 6 
FoodServicePin = 14

# Relay signal output pins to switch the relays on or off
FrontDoorRelayPin = 16
ClosetDoorRelayPin = 20
FoodServiceRelayPin = 12
ExitDoorRelayPin = 21

time.sleep(0.1)
# Set all receiving signal pins to high / pull up
pi.set_pull_up_down(ResetPin, pigpio.PUD_UP)  # reset switch
pi.set_pull_up_down(FrontDoorPin, pigpio.PUD_UP)        # front door reed switch pin
pi.set_pull_up_down(ClosetDoorPin, pigpio.PUD_UP)       # closet door reed switch pin
pi.set_pull_up_down(FoodServicePin, pigpio.PUD_UP)       # food service maglock switch pin


time.sleep(0.1)
pi.set_mode(FrontDoorRelayPin, pigpio.OUTPUT)   #smoke machine relay pin
pi.set_mode(ClosetDoorRelayPin, pigpio.OUTPUT)   #smoke machine relay pin
pi.set_mode(FoodServiceRelayPin, pigpio.OUTPUT)   #smoke machine relay pin
pi.set_mode(ExitDoorRelayPin, pigpio.OUTPUT)

pi.set_glitch_filter(FilterPin, 1000)

def printAll():
    print("Front: ", pi.read(FrontDoorPin))
    print("Closet: ", pi.read(ClosetDoorPin))
    print("Food: ", pi.read(FoodServicePin))

printAll()
# callback function to re-engage maglocks when the reset pin is low
def callbackReset(gpio, level, tick):
    if pi.read(ResetPin) == 0:
        resetRelays();

def resetRelays():
        print('Reset called')
        printAll()
        pi.write(FrontDoorRelayPin, 1)
        pi.write(ClosetDoorRelayPin, 1)
        pi.write(FoodServiceRelayPin, 1)
        pi.write(ExitDoorRelayPin, 1)
        pi.write(ResetPin, 1);
        try:
            print(requests.get("http://darkside-guitar:15005/reset"))
        except:
            print(traceback.format_exc())

# callback function listening for low power on signal pin
def callbackFrontDoor(gpio, level, tick):
    if pi.read(FrontDoorPin) == 0:
        printAll()
        print("Front door opened")
        pi.write(FrontDoorRelayPin, 0)

def callbackClosetDoor(gpio, level, tick):
    if pi.read(ClosetDoorPin) == 0:
        printAll()
        print("Closet door opened")
        pi.write(ClosetDoorRelayPin, 0)

def callbackFoodService(gpio, level, tick):
    if pi.read(FoodServicePin) == 0:
        printAll()
        print("Room service called")
        try:
            print(requests.get("http://darkside-guitar:15005/room-service"))
        except:
            print(traceback.format_exc())

        time.sleep(3)
        pi.write(FoodServiceRelayPin, 0)

time.sleep(0.1)
cb1 = pi.callback(ResetPin, pigpio.FALLING_EDGE, callbackReset)
cb2 = pi.callback(FrontDoorPin, pigpio.FALLING_EDGE, callbackFrontDoor)
cb3 = pi.callback(ClosetDoorPin, pigpio.FALLING_EDGE, callbackClosetDoor)
cb4 = pi.callback(FoodServicePin, pigpio.FALLING_EDGE, callbackFoodService)
printAll()

class requestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('content-type', 'text/html')
        self.end_headers()
        request = self.path[1:]
        if (request == "open-exit"):
            pi.write(ExitDoorRelayPin, 0)
        elif (request == "open-front-door"):
            pi.write(FrontDoorRelayPin, 0)
        elif (request == "open-closet-door"):
            pi.write(ClosetDoorRelayPin, 0)
        elif (request == "room-service"):
            try: 
                print(requests.get("http://darkside-guitar:15005/room-service"))
            except:
                print("can't connect to guitar raspi")
            
            time.sleep(5)
            pi.write(FoodServiceRelayPin, 0)
        elif (request == "reset"):
            resetRelays()
        else:
            self.send_error(500, "Server error")

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(("8.8.8.8",80))
ip = s.getsockname()[0]
s.close()

print("Attempting Connection")
print(ip)

server_address = (ip, PORT)
server = HTTPServer(server_address, requestHandler)
    
print("Server running")
printAll()
try:
    resetRelays()
    time.sleep(0.1)
    server.serve_forever()
except Exception:
    print(traceback.format_exc())
    pi.write(FrontDoorRelayPin, 0)
    pi.write(ClosetDoorRelayPin, 0)
    pi.write(FoodServiceRelayPin, 0)
    pi.write(ExitDoorRelayPin, 0)
    pi.stop()
    sys.exit()
