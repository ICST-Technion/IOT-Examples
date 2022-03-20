# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import numpy as np
font=cv2.FONT_HERSHEY_SIMPLEX

############select here##################
bright_thresh=180
##############select here################

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 480))

# allow the camera to warmup
time.sleep(0.1)

# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    # grab the raw NumPy array representing the image, then initialize the timestamp
    # and occupied/unoccupied text
    img = frame.array
    #////////prepare image for contour finding////////////
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    negative = cv2.bitwise_not(gray)

    #//////simple thresholding
    ret,thresh = cv2.threshold(negative,bright_thresh,255,cv2.THRESH_BINARY)
            
    cv2.putText(negative, str(ret) ,(0,30), font, 1,(255,255,255),2)
    cv2.imshow('thresh', thresh)
    
    #//////contour finding and drawing on the image//////////
    (_,contours,_) = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    
    if len(contours)>0:
        print("found %s contours" % (len(contours)))
        for contour in contours[:3]:
            rect = cv2.boundingRect(contour)
            cv2.rectangle(img, (rect[0],rect[1]), (rect[2]+rect[0],rect[3]+rect[1]), (0,255,0), 3)
            cv2.drawContours(img, contours, -1, (0,0,255), 2)
            rect_min = cv2.minAreaRect(contour)
            box = cv2.boxPoints(rect_min)
            box = np.int0(box)
            cv2.drawContours(img,[box],0,(255,0,0),2)
            print("angle= ",rect_min[2])
    else:
        print("no contours found")

    #/////////////display images////////////////
    cv2.imshow('negative', negative)
    cv2.imshow('img_rect', img)
    cv2.waitKey(20)
    rawCapture.truncate(0)
    


