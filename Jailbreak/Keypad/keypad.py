from houdini import switchRelayState, buttonpressed


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

ROW = [35,21,23,31]
COL = [33,37,29]
correct_code = [5,9,1,3]
input = []
button_input_pin = 22
# button_supply_pin = 26

GPIO.setup(button_input_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
#


# GPIO.setup(button_supply_pin, GPIO.OUT)


for j in range(3):
    GPIO.setup(COL[j], GPIO.OUT)
    GPIO.output(COL[j], 1)

for i in range(4):
    GPIO.setup(ROW[i], GPIO.IN, pull_up_down = GPIO.PUD_UP)


while True:
    for j in range(3):
        GPIO.output(COL[j],0)
        
        for i in range(4):
            
            if GPIO.input(ROW[i]) == 0:
                time.sleep(0.05) ##debounce
                if GPIO.input(ROW[i]) == 0:
                    print (MATRIX[i][j])
                    if MATRIX[i][j] == '*':
                        input = []
                    else:
                        input.append(MATRIX[i][j])
                    time.sleep(0.1) ##????
                    while(GPIO.input(ROW[i]) == 0):
                        pass
        GPIO.output(COL[j],1)
    if len(input) == 4:
        if input == correct_code:
            time.sleep(0.5)
            switchRelayState("relay_7")
            print("correct code")
             
        else:
            print("incorrect code, waiting for *")
            ##wait in this loop for * to be pressed. then reset input to []
            while True:
                GPIO.output(COL[2],0)
                if GPIO.input(ROW[3]) == 0:  # Check if '*' is pressedpygame.init()
                    time.sleep(0.05)
                    if GPIO.input(ROW[3]) == 0:
                        input = []  # Reset input
                        print(MATRIX[i][j])
                        time.sleep(0.1)
                        while(GPIO.input(ROW[3])) == 0:
                            pass
                        GPIO.output(COL[2],1)
                        break
                
    if GPIO.input(button_input_pin) == 0:
        print("Button pressed")
        buttonpressed()
        break
