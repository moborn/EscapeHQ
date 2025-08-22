from pi_replacement import switchRelayState, stopSound


import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)
GPIO.setwarnings(False)

MATRIX = [
     [1,2,3],
     [4,5,6],
     [7,8,9],
     ['*',0,'#']
]

ROW = [3,5,8,10]
COL = [19,21,23]
correct_code = [5,9,1,3]
input = []
button_input_pin = 24
# button_supply_pin = 26

GPIO.setup(button_input_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
# GPIO.setup(button_supply_pin, GPIO.OUT)


for j in range(4):
    GPIO.setup(COL[j], GPIO.OUT)
    GPIO.output(COL[j], 1)

for i in range(4):
    GPIO.setup(ROW[i], GPIO.IN, pull_up_down = GPIO.PUD_UP)


while True:
    for j in range(3):
        GPIO.output(COL[j],0)
        
        for i in range(4):
            if GPIO.input(ROW[i]) == 0:
                print (MATRIX[i][j])
                input.append(MATRIX[i][j])
                time.sleep(0.1) ##????
                while(GPIO.input(ROW[i]) == 0):
                    pass
    if len(input) == 4:
        if input == correct_code:
            switchRelayState("relay_7")
        else:
            ##wait in this loop for * to be pressed. then reset input to []
            while True:
                GPIO.output(COL[0],0)
                if GPIO.input(ROW[3]) == 0:  # Check if '*' is pressed
                    input = []  # Reset input
                    break

        GPIO.output(COL[j],1)
    if GPIO.input(button_input_pin) == 0:
        print("Button pressed")
        stopSound()
    
