#!/usr/bin/python
# Imports
import RPi.GPIO as GPIO
from time import time
from time import sleep
from datetime import datetime

# Pin Definitions
bcdD   = 5  # binary coded decimal (BCD) least significant bit (LSB) for 74LS47 A input pin
bcdA   = 19
bcdB   = 13
bcdC   = 6  # binary coded decimal (BCD) most significant bit (MSB) for 74LS47 D input pin
blank  = 4
le     = 26
#d[1]=27;d[6]=22;d[5]=17;d[4]=10;d[3]=9;d[2]=11;
d     = [0,27,22,17,10,9,11]

# Global Variables
count = [0,1,2,3,4,5,6 ] 
dig=1
t=time()                                      # reset seconds timer
disp_interval=.003                            # sleep time before next digit is multiplexed
second_interval=1                             # no. seconds before next time update

# Functions
def udatetime():                              # update hrs,mins,secs on display
  global count
  mytime = datetime.now().strftime("%H%M%S")
  count = [0,int(mytime[5]),int(mytime[4]),int(mytime[3]),int(mytime[2]),int(mytime[1]),int(mytime[0])]

# Reads a bit of a number
def bitRead(value, bit):
   return value & (1 << bit)                  # shift mask to bit position and AND to value

def displayWrite():                           # Multiplex display digits  
   global dig  
   GPIO.output(d[dig], 0)                     # turn off previous digit
   dig=dig+1                                  # increment to next digit
   if dig > 6:
      dig = 1
   GPIO.output(bcdA, bitRead(count[dig], 0))  # send next digit value
   GPIO.output(bcdB, bitRead(count[dig], 1))
   GPIO.output(bcdC, bitRead(count[dig], 2))
   GPIO.output(bcdD, bitRead(count[dig], 3))  
   GPIO.output(d[dig], 1)                     # turn on next digit

# Main
# Pin configuration
GPIO.setmode(GPIO.BCM)                        # use BCM pin numbering(GPIO)
GPIO.setup(bcdA, GPIO.OUT)
GPIO.setup(bcdB, GPIO.OUT)
GPIO.setup(bcdC, GPIO.OUT)
GPIO.setup(bcdD, GPIO.OUT)
GPIO.setup(blank, GPIO.OUT)
GPIO.setup(le, GPIO.OUT)
for i in range (1,7):
   GPIO.setup(d[i], GPIO.OUT)
GPIO.output(le, 1)                            # latch enable is always on
GPIO.output(blank, 0)                         # blanking is always off

print("Press CTRL-C to exit.")
try:
   while True:
      displayWrite()                # update display
      sleep(disp_interval)          # wait for next display update 
      if time()-t>second_interval:  # next second update
         #run task
         udatetime()
         t=time()                   # reset seconds timer
# Cleanup
finally:           # exit cleanly when CTRL+C is pressed
   GPIO.cleanup()  # release all GPIO resources
   print("\nCompleted cleanup of GPIO resources.")

