'''
    Author: Matt Cassidy
    Date:   19/05/2020

    Brief:  A Python-based interface between the Houdini MC Software 
            and the electronic room puzzles at Escape HQ, Takapuna. 

    GPIO Pinouts (Note: See pinout.xyz for WiringPi pinouts):

                       3.3V  o---  1 | 2  ---o  5.0V
       I2C1_SDA  |  GPIO.02  o---  3 | 4  ---o  5.0V
       I2C1_SCL  |  GPIO.03  o---  5 | 6  ---o  Ground
      Signal.01 <<< GPIO.04  o---  7 | 8  ---o  GPIO.14  |  UART_TXD
                     Ground  o---  9 | 10 ---o  GPIO.15  |  UART_RXD
      Signal.02 <<< GPIO.17  o--- 11 | 12 ---o  GPIO.18 >>> Relay.01  'Room Door'
      Signal.03 <<< GPIO.27  o--- 13 | 14 ---o  Ground
      Signal.04 <<< GPIO.22  o--- 15 | 16 ---o  GPIO.23 >>> Relay.02  'Ski Poles'
                       3.3V  o--- 17 | 18 ---o  GPIO.24 >>> Relay.03  'Window'
      SPIO_MOSI  |  GPIO.10  o--- 19 | 20 ---o  Ground
      SPIO_MISO  |  GPIO.09  o--- 21 | 22 ---o  GPIO.25 >>> Relay.04  'Handprint'
      SPIO_SCLK  |  GPIO.11  o--- 23 | 24 ---o  GPIO.08  |  SPIO_CE0
                     Ground  o--- 25 | 26 ---o  GPIO.07  |  SPIO_CE1
                   RESERVED  x--- 27 | 28 ---x  RESERVED
      Signal.05 <<< GPIO.05  o--- 29 | 30 ---o  Ground
      Signal.06 <<< GPIO.06  o--- 31 | 32 ---o  GPIO.12 >>> Relay.05  'Keypad'
      Signal.07 <<< GPIO.13  o--- 33 | 34 ---o  Ground
      Signal.08 <<< GPIO.19  o--- 35 | 36 ---o  GPIO.16 >>> Relay.06  'Hut Power'
      Signal.09 <<< GPIO.26  o--- 37 | 38 ---o  GPIO.20 >>> Relay.07  'Drop-Drawer'
                     Ground  o--- 39 | 40 ---o  GPIO.21 >>> Relay.08  'Candles'

    Copyright 2020, mattc.creativesolutions@gmail.com
 '''
# python imports
from sched import scheduler
from urllib.request import urlopen
import time, pygame

# GPIO setup
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

# global variables
print_debug_output = True
game_in_progress = False
game_is_complete = True
schedule_reset = False

# Houdini configuration
houdini = "http://192.168.178.1:14999/"
reset_delay = 300.0

# class definition -> GPIO pinouts
class Pinout:
    def __init__(self, gpio, setup, state, name):
        self.name = name
        self.gpio = gpio
        self.setup = setup
        self.state = state
        self.init_state = state
        self.changed = False
        if self.setup == GPIO.IN:
            GPIO.setup(self.gpio, self.setup, pull_up_down=GPIO.PUD_DOWN)
        if self.setup == GPIO.OUT:
            GPIO.setup(self.gpio, self.setup)
            GPIO.output(self.gpio, self.state)
    # GET input state   (1 = HIGH, 0 = LOW)
    def getState(self):
        if self.setup == GPIO.IN:
            state = GPIO.input(self.gpio)
            self.changed = self.state != state
            self.state = state
        return self.state
    # SET output state  (1 = HIGH, 0 = LOW)
    def setState(self, state):
        if self.setup == GPIO.OUT:
            self.changed = self.state != state
            self.state = state
            GPIO.output(self.gpio, self.state)
    # SET output to initial state
    def resetOUT(self):
        if self.setup == GPIO.OUT:
            self.changed = self.state != self.init_state
            self.state = self.init_state
            GPIO.output(self.gpio, self.state)
        
# physical GPIO pinouts (BCM)
p07 = Pinout(4,  1, 0, "input_1")
p11 = Pinout(17, 1, 0, "input_2")
p12 = Pinout(18, 0, 0, "relay_1")
p13 = Pinout(27, 1, 0, "input_3")
p15 = Pinout(22, 1, 0, "input_4")
p16 = Pinout(23, 0, 0, "relay_2")
p18 = Pinout(24, 0, 0, "relay_3")
p22 = Pinout(25, 0, 0, "relay_4")
p29 = Pinout(5,  1, 0, "input_5")
p31 = Pinout(6,  1, 0, "input_6")
p32 = Pinout(12, 0, 0, "relay_5")
p33 = Pinout(13, 1, 0, "input_7")
p35 = Pinout(19, 1, 0, "input_8")
p36 = Pinout(16, 0, 0, "relay_6")
p37 = Pinout(26, 1, 0, "input_9")
p38 = Pinout(20, 0, 0, "relay_7")
p40 = Pinout(21, 0, 0, "relay_8")

# object lists
relays = [p12, p16, p18, p22, p32, p36, p38, p40]
inputs = [p36, p38, p40]

# >> FUNCTIONS <<
def debug_print(a='-'):
    if print_debug_output:
        print("debug output", time.time(), a)
        
def switch_relay_state(name, state):
    for r in relays:
        if r.name == name:
            r.setState(state)
            # notify Houdini of the change
            msg = urlopen(houdini + r.name).read()
            debug_print("{} is {}".format(r.name, state))
            time.sleep(0.1)
            break

def new_game():
    global game_in_progress, game_is_complete
    if not game_in_progress and game_is_complete:
        debug_print("ready for new game..")
        # start the game when the room door closes
        inputs[0].getState()
        if inputs[0].changed:
            # notify Houdini to >> START << the Clock
            msg = urlopen(houdini + "start").read()
            game_in_progress = True
            game_is_complete = False
            debug_print("game started")
    
def game_loop():
    global game_in_progress, game_is_complete
    if game_in_progress and not game_is_complete:
        # read all GPIO input states
        for i in inputs:
            i.getState()

        # check if the room door is open
        if inputs[0].changed:
            # notify Houdini the emergency exit is open
            msg = urlopen(houdini + inputs[0].name).read()
            debug_print("warning: emergency exit open")

        # check if the fusebox puzzle is done
        if inputs[1].changed:
            # ENABLE power to 'hut_power'
            switch_relay_state("hut_power", 0)
            # notify Houdini the fusebox puzzle is done
            msg = urlopen(houdini + inputs[1].name).read()
            debug_print("fusebox solved")
            
        # check if the flare puzzle is done
        if inputs[2].changed:
            # ENABLE power to 'flare_LED'
            switch_relay_state("flare_LED", 0)
            # notify Houdini the flare puzzle is done
            msg = urlopen(houdini + inputs[2].name).read()
            game_is_complete = True
            debug_print("flare lit")

def end_game():
    global game_in_progress, game_is_complete, schedule_reset
    if game_in_progress and game_is_complete:
        # run end game sequence
        # Play helicopter.mp3
        time.sleep(2.0)
        # DISABLE power to 'room_door'
        switch_relay_state("room_door", 1)
        # notify Houdini to ->> STOP <<- the Clock
        msg = urlopen(houdini + "stop").read()
        game_in_progress = False
        schedule_reset = True
        debug_print("game complete")
    
def reset_game():
    global game_in_progress, game_is_complete, schedule_reset
    # automatically reset the room
    for r in relays:
        r.resetOUT()
    # notify Houdini the room has been reset
    msg = urlopen(houdini +"reset").read()
    game_in_progress = False
    game_is_complete = True
    schedule_reset = False
    debug_print("reset complete")
    
# >> MAIN ROUTINE <<
try:
    while(True):
        s = scheduler(time.time, time.sleep)

        s.enter(1, 1, new_game)
        s.enter(1, 2, game_loop)
        s.enter(1, 3, end_game)
        s.run()
        
        if schedule_reset:
            debug_print("reset scheduled")
            s.enter(reset_delay, 1, reset_game)
            s.run()
            
except (KeyboardInterrupt, SystemExit):
    raise
except:
    GPIO.cleanup()
    exit()