from picamera2 import Picamera2
from Hand_Gesture_Recognition import *
import cv2
import time

picam2 = Picamera2()
video_config = picam2.create_preview_configuration({"format":"XRGB8888","size": (640,480)})
#picam2.video_configuration.controls.FrameRate = 5.0
picam2.configure(video_config)
picam2.start_preview()
picam2.start()
time.sleep(5)
picam2.capture_file("Capture1.png")

if True:
    im = picam2.capture_array()
    print("Time stamp start capture: " + str(time.time_ns()))
    imdata = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)
    init_recognize(imdata)
    #cv2.imwrite("Capture.png", imdata)
    #cv2.imshow("Real Time Capture",im)
    #time.sleep(1)
    
