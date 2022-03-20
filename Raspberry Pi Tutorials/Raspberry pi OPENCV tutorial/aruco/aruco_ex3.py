import numpy as np
import cv2
import cv2.aruco as aruco
from picamera.array import PiRGBArray
from picamera import PiCamera


my_id=1

aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250)
#aruco_dict = aruco.Dictionary_get(aruco.DICT_5X5_1000)
calib=np.load('bad_calib.npy')
#calib=np.load('good_calib.npy')
mtx=calib[0]
dist=calib[1]

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
    #print "found" ,len(corners) , "markers"
    if len(corners):
        if my_id in ids:
            index=np.where(ids == my_id)[0][0]
            print "found marker" , my_id , " at index" , index
            mycorners=corners[index]
            rvec, tvec , _objPoints = aruco.estimatePoseSingleMarkers(mycorners, 0.08, mtx, dist) #Estimate pose of each marker and return the values rvet and tvec---different from camera coeficcients
            (rvec-tvec).any() # get rid of that nasty numpy value array error
        
            aruco.drawAxis(img, mtx, dist, rvec, tvec, 0.1) #Draw Axis
            #print "blue axis at ", rvec [0][0][0]
            print "red axis at ", rvec [0][0][1]

    aruco.drawDetectedMarkers(img, corners, ids , (255,255,128))
    
    # Display the resulting frame
    cv2.imshow('found',img)
    cv2.waitKey(10)
    rawCapture.truncate(0)
    
    
# When everything done, release the capture
cv2.destroyAllWindows()
