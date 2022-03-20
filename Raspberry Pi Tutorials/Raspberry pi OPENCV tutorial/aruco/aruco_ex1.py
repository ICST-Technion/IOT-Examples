import numpy as np
import cv2
import cv2.aruco as aruco
from picamera.array import PiRGBArray
from picamera import PiCamera



aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250)
#aruco_dict = aruco.Dictionary_get(aruco.DICT_5X5_1000)

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 32
rawCapture = PiRGBArray(camera, size=(640, 480))

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):

    img = frame.array

    # convert to gray
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    parameters =  aruco.DetectorParameters_create()

    '''    detectMarkers(...)
        detectMarkers(image, dictionary[, corners[, ids[, parameters[, rejectedI
        mgPoints]]]]) -> corners, ids, rejectedImgPoints
        '''
    #lists of ids and the corners beloning to each id
    corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)
    print "found" ,len(corners) , "markers"
    if len(corners)>0:
        print corners[0]
    aruco.drawDetectedMarkers(img, corners, ids , (255,255,128))

    # Display the resulting frame
    cv2.imshow('found',img)
    cv2.waitKey(10)
    rawCapture.truncate(0)
    
    
# When everything done, release the capture
cv2.destroyAllWindows()
