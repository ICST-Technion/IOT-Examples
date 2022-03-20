import time
import cv2
import cv2.aruco as A
import numpy as np
import picamera
import picamera.array


dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
board = cv2.aruco.CharucoBoard_create(3,3,.025,.0125,dictionary)
img = board.draw((200*3,200*3))

#Dump the calibration board to a file
#cv2.imwrite('charuco.png',img)

allCorners = []
allIds = []
decimator = 0

for i in range(4):
    with picamera.PiCamera() as camera:
        with picamera.array.PiRGBArray(camera) as stream:
            camera.resolution = (640, 480)
            camera.capture(stream, format='bgr')
            # At this point the image is available as stream.array
            frame = stream.array

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    res = cv2.aruco.detectMarkers(gray,dictionary)

    if len(res[0])>0:
        res2 = cv2.aruco.interpolateCornersCharuco(res[0],res[1],gray,board)
        if res2[1] is not None and res2[2] is not None and len(res2[1])>3 and decimator%3==0:
            allCorners.append(res2[1])
            allIds.append(res2[2])

        cv2.aruco.drawDetectedMarkers(gray,res[0],res[1])

    cv2.imshow('frame',gray)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    decimator+=1
    print i

imsize = gray.shape

#Calibration fails for lots of reasons. Release the video if we do
try:
    cal = cv2.aruco.calibrateCameraCharuco(allCorners,allIds,board,imsize,None,None)
    print "calibration sucess"
except:
    print "error in calibration"

calsave= [cal[1], cal[2]]
np.save('new_calib.npy',calsave)

##cameraMatrix=cal[1]
##distCoeffs=cal[2]

cv2.destroyAllWindows()
