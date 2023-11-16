import cv2
from Hand_Gesture_Recognition import *

imdata = cv2.imread("Capture1.png")
imdata = cv2.cvtColor(imdata, cv2.COLOR_BGR2RGB)
print("Sent")
init_recognize(imdata)

if True:
    print("Waiting for Data")
