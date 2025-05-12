import sys
import time  #need this for sleep
import pigpio
pi = pigpio.pi()
time.sleep(1)
closetPin = 25
print("helo")
pi.set_pull_up_down(closetPin, pigpio.PUD_UP)
pi.write(closetPin, 1)
def cab(gpio, level, tick):
    print(gpio, level, tick)
    print(pi.read(closetPin))

cb = pi.callback(closetPin, pigpio.EITHER_EDGE, cab)

while(True):
    try:
        print(pi.read(closetPin))
        time.sleep(1)
    except:
        pi.stop()
        exit()
