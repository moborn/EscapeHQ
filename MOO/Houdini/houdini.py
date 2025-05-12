
# Toggle relays from houdini
# Written by Jouveer Naidoo - jouveer@gmail.com - 19/09/2022

# GPIO Pinouts (Note: See pinout.xyz for WiringPi pinouts):

#                    3.3V  o---  1 | 2  ---o  5.0V
#    I2C1_SDA  |  GPIO.02  o---  3 | 4  ---o  5.0V
#    I2C1_SCL  |  GPIO.03  o---  5 | 6  ---o  Ground
#   Signal.01 <<< GPIO.04  o---  7 | 8  ---o  GPIO.14  |  UART_TXD
#                  Ground  o---  9 | 10 ---o  GPIO.15  |  UART_RXD
#   Signal.02 <<< GPIO.17  o--- 11 | 12 ---o  GPIO.18 >>> Relay.01
#   Signal.03 <<< GPIO.27  o--- 13 | 14 ---o  Ground
#   Signal.04 <<< GPIO.22  o--- 15 | 16 ---o  GPIO.23 >>> Relay.02 
#                    3.3V  o--- 17 | 18 ---o  GPIO.24 >>> Relay.03 
#   SPIO_MOSI  |  GPIO.10  o--- 19 | 20 ---o  Ground 
#   SPIO_MISO  |  GPIO.09  o--- 21 | 22 ---o  GPIO.25 >>> Relay.04 
#   SPIO_SCLK  |  GPIO.11  o--- 23 | 24 ---o  GPIO.08  |  SPIO_CE0 
#                  Ground  o--- 25 | 26 ---o  GPIO.07  |  SPIO_CE1 
#                RESERVED  x--- 27 | 28 ---x  RESERVED 
#   Signal.05 <<< GPIO.05  o--- 29 | 30 ---o  Ground 
#   Signal.06 <<< GPIO.06  o--- 31 | 32 ---o  GPIO.12 >>> Relay.05 
#   Signal.07 <<< GPIO.13  o--- 33 | 34 ---o  Ground 
#   Signal.08 <<< GPIO.19  o--- 35 | 36 ---o  GPIO.16 >>> Relay.06
#   Signal.09 <<< GPIO.26  o--- 37 | 38 ---o  GPIO.20 >>> Relay.07
#                  Ground  o--- 39 | 40 ---o  GPIO.21 >>> Relay.08

import socket
from http.server import HTTPServer, BaseHTTPRequestHandler
import logging
import RPi.GPIO as GPIO
logging.basicConfig(filename='relayLog.txt', level = logging.DEBUG, format='%(asctime)s %(message)s')

#Logging
def log(message):
    if(DEBUG):
        logging.debug(message)

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
DEBUG = False
PORT = 15004

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
 
# Physical GPIO pinouts (BCM) 
p12 = Relay(18, 0, "relay_1")  # Exit Door 
p16 = Relay(23, 0, "relay_2")  # Insemination 
p18 = Relay(24, 0, "relay_3")  # Left Farm Panel
p22 = Relay(25, 0, "relay_4")  # Right Farm Panel
p32 = Relay(12, 0, "relay_5")  # Prize Cow
p36 = Relay(16, 0, "relay_6")  # Breakfast Doors
p38 = Relay(20, 0, "relay_7")  # Entry Door
p40 = Relay(21, 0, "relay_8")  # 

p18 = Relay(24, 0, "relay_3")  # Left Farm Panel
p22 = Relay(25, 0, "relay_4")  # Right Farm Panel
p32 = Relay(12, 0, "relay_5")  # Prize Cow
p36 = Relay(16, 0, "relay_6")  # Breakfast Doors
p38 = Relay(20, 0, "relay_7")  # Entry Door
p40 = Relay(21, 0, "relay_8")  # 
 
relays = [p12,p16,p18,p22,p32,p36,p38,p40] 
 
def switchRelayState(name): 
    for relay in relays: 
        if relay.getName() == name: 
            state = relay.getState() != 1 
            relay.setState(state) 
            print("Setting relay {} to {}".format(name, state)) 
            log("{} is {}".format(relay.getName(), relay.getState())) 
            break

class requestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        try:
            self.send_response(200) 
            self.send_header('content-type', 'text/html')
            self.end_headers()
            request = self.path[1:]
            if 'relay' in request : switchRelayState(request)
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
