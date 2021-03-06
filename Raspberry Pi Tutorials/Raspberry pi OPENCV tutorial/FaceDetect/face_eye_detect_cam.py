#https://realpython.com/face-recognition-with-python/
# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np


# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 400)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 400))

# Create the haar cascade
faceCascade = cv2.CascadeClassifier("haarcascade_frontalface_default.xml")
#eye_cascade = cv2.CascadeClassifier('haarcascade_eye.xml')
eye_cascade = cv2.CascadeClassifier('haarcascade_lefteye_2splits.xml')

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

    print("Found {0} faces!".format(len(faces)))

    # Draw a rectangle around the faces
    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = img[y:y+h, x:x+w]

        eyes = eye_cascade.detectMultiScale(roi_gray,
                                            scaleFactor=1.2,
                                            minNeighbors=3,
                                            maxSize=(50, 50)
                                            )
        
        print("*** Found {0} eyes!".format(len(eyes)))
        for (ex,ey,ew,eh) in eyes:
            cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(255,0,0),2)

    # Display the resulting frame
    cv2.imshow('image', img)
    
    endt=time.time()
    fps=np.append(fps,(1/(endt-startt)))
    if len(fps)>10:
        print "FPS=",np.mean(fps)
        fps=[]
    cv2.waitKey(10)
    rawCapture.truncate(0)




    


