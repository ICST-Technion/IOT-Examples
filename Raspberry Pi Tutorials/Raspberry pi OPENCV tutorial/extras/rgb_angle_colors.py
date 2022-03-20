#defining the RPi's pins as Input / Output
import RPi.GPIO as GPIO
#importing the library for delaying command.
import time

# Import the ADXL345 module.
import Adafruit_ADXL345

# Create an ADXL345 instance.
accel = Adafruit_ADXL345.ADXL345()

#used for GPIO numbering
GPIO.setmode(GPIO.BCM) 

#closing the warnings when you are compiling the code
GPIO.setwarnings(False)

RUNNING = True

#defining the pins
green = 20
red = 21
blue = 16

leds = { 0 : 21, 1 : 20, 2: 16 }
i=0

#defining the pins as output
GPIO.setup(red, GPIO.OUT) 
GPIO.setup(green, GPIO.OUT)
GPIO.setup(blue, GPIO.OUT)
GPIO.setup(13, GPIO.IN, pull_up_down=GPIO.PUD_UP)

#choosing a frequency for pwm
Freq = 100

#defining the pins that are going to be used with PWM
RED = GPIO.PWM(red, Freq)  
GREEN = GPIO.PWM(green, Freq)
BLUE = GPIO.PWM(blue, Freq)

RED.start(1)
GREEN.start(1) 
BLUE.start(1)
try:
        #we are starting with the loop
        while RUNNING: 
                # Read the X, Y, Z axis acceleration values and print them.
                x, y, z = accel.read()
                print('X={0}, Y={1}, Z={2}'.format(x, y, z))
                if x < 0:
                        x = x * (-1)
                if y < 0:
                        y = y * (-1)
                if z < 0:
                        z = z * (-1)
                if x > 240:
                        x = 240
                if y > 240:
                        y = 240
                if z > 240:
                        z = 240
                print('X\'={0}, Y\'={1}, Z\'={2}'.format(x, y, z))
                
                RED.ChangeDutyCycle(x*100/240)
                GREEN.ChangeDutyCycle(y*100/240)
                BLUE.ChangeDutyCycle(z *100/240)

                # Wait half a second and repeat.
                time.sleep(0.01)

        


except KeyboardInterrupt: 
        # the purpose of this part is, when you interrupt the code, it will stop the while loop and turn off the pins, which means your LED won't light anymore
        RUNNING = False  
        GPIO.cleanup()
