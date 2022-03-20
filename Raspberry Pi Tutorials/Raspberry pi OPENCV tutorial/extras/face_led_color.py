#https://realpython.com/face-recognition-with-python/
# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np
import RPi.GPIO as GPIO # always needed with RPi.GPIO
from time import sleep

###### init colors
bluepin=23
redpin=24
greenpin=25

GPIO.setmode(GPIO.BCM)  # choose BCM or BOARD numbering schemes. I use BCM

GPIO.setup(bluepin, GPIO.OUT)# set GPIO 25 as output for white led
GPIO.setup(redpin, GPIO.OUT)# set GPIO 24 as output for red led
GPIO.setup(greenpin, GPIO.OUT)# set GPIO 24 as output for red led


blue = GPIO.PWM(bluepin, 100)    # create object white for PWM on port 25 at 100 Hz
red = GPIO.PWM(redpin, 100)      # create object red for PWM on port 24 at 100Hertz
green = GPIO.PWM(greenpin, 100)
blue.start(0)              # start white led on 0 percent duty cycle (off)
red.start(0)
green.start(0)
# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 400)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 400))

# Create the haar cascade
faceCascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
#eye_cascade = cv2.CascadeClassifier('haarcascade_eye.xml')
#eye_cascade = cv2.CascadeClassifier('haarcascade_lefteye_2splits.xml')

fps=[]

# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    
    startt=time.time()
    img = frame.array

    # Our operations on the frame come here
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Detect faces in the image
    faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.4,
            minNeighbors=2,
            minSize=(30, 30)
            #flags = cv2.CV_HAAR_SCALE_IMAGE
    )


        
    # Draw a rectangle around the faces
    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = img[y:y+h, x:x+w]


    print("Found {0} faces!".format(len(faces)))
    if len(faces)>0:
        faces=faces[0] #if multiple faces pick the first instance
        print faces[3] #debug

        if faces[3]< 70: 
            blue.ChangeDutyCycle(0)
            red.ChangeDutyCycle(100)
            green.ChangeDutyCycle(0)
        elif faces[3]<150:
            blue.ChangeDutyCycle(0)
            red.ChangeDutyCycle(100)
            green.ChangeDutyCycle(0)

    else:
        blue.ChangeDutyCycle(0)
        red.ChangeDutyCycle(0)
        green.ChangeDutyCycle(0)

        #eyes = eye_cascade.detectMultiScale(roi_gray,
                                    #        scaleFactor=1.2,
                                     #       minNeighbors=3,
                                      #      maxSize=(50, 50)
                                       #     )
        
        #print("*** Found {0} eyes!".format(len(eyes)))
        #for (ex,ey,ew,eh) in eyes:
         #   cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(255,0,0),2)

    # Display the resulting frame
    cv2.imshow('image', img)
    
    endt=time.time()
    fps=np.append(fps,(1/(endt-startt)))
    if len(fps)>10:
        print "FPS=",np.mean(fps)
        fps=[]
    cv2.waitKey(10)
    rawCapture.truncate(0)



################################################################################
            # red fully on (100%)

# now the fun starts, we'll vary the duty cycle to 
# dim/brighten the leds, so one is bright while the other is dim

pause_time = 0.02           # you can change this to slow down/speed up

try:
    while True:
        for i in range(0,101):      # 101 because it stops when it finishes 100
            blue.ChangeDutyCycle(i)
            sleep(pause_time)
        for i in range(100,-1,-1):      # from 100 to zero in steps of -1
            red.ChangeDutyCycle(i)
            sleep(pause_time)

except KeyboardInterrupt:
    white.stop()            # stop the white PWM output
    blue.stop()              # stop the red PWM output
    GPIO.cleanup()          # clean up GPIO on CTRL+C exit

##############################################################################

    


