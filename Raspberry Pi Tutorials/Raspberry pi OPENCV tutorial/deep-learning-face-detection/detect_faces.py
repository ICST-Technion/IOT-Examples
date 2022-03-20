# USAGE
# python detect_faces_video.py --prototxt deploy.prototxt.txt --model res10_300x300_ssd_iter_140000.caffemodel

# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera

import numpy as np
import argparse
import imutils
import time
import cv2

# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 16
rawCapture = PiRGBArray(camera, size=(640, 480))


prototxt='deploy.prototxt.txt'
model='res10_300x300_ssd_iter_140000.caffemodel'
confidence_level=0.4

# load our serialized model from disk
print("[INFO] loading model...")
net = cv2.dnn.readNetFromCaffe(prototxt, model)

# initialize the video stream and allow the cammera sensor to warmup
print("[INFO] starting video stream...")
time.sleep(1.0)

# loop over the frames from the video stream
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):

	# grab the frame from the threaded video stream and resize it
	# to have a maximum width of 400 pixels
	inframe = frame.array
	inframe = imutils.resize(inframe, width=400)
 
	# grab the frame dimensions and convert it to a blob
	(h, w) = inframe.shape[:2]
	blob = cv2.dnn.blobFromImage(cv2.resize(inframe, (300, 300)), 1.0,
		(300, 300), (104.0, 177.0, 123.0))
 
	# pass the blob through the network and obtain the detections and
	# predictions
	net.setInput(blob)
	detections = net.forward()

	# loop over the detections
	for i in range(0, detections.shape[2]):
		# extract the confidence (i.e., probability) associated with the
		# prediction
		confidence = detections[0, 0, i, 2]

		# filter out weak detections by ensuring the `confidence` is
		# greater than the minimum confidence
		if confidence < confidence_level:
			continue

		# compute the (x, y)-coordinates of the bounding box for the
		# object
		box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
		(startX, startY, endX, endY) = box.astype("int")
 
		# draw the bounding box of the face along with the associated
		# probability
		text = "{:.2f}%".format(confidence * 100)
		y = startY - 10 if startY - 10 > 10 else startY + 10
		cv2.rectangle(inframe, (startX, startY), (endX, endY),
			(0, 0, 255), 2)
		cv2.putText(inframe, text, (startX, y),
			cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 255), 2)

	# show the output frame
	cv2.imshow("Frame", inframe)
	key = cv2.waitKey(1) & 0xFF
	rawCapture.truncate(0)
         
	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break

# do a bit of cleanup
cv2.destroyAllWindows()

