import cv2
from Hand_Gesture_Recognition import *

imdata = cv2.imread("test_resource/hand.jpg")
imdata = cv2.cvtColor(imdata, cv2.COLOR_BGR2RGB)
print("Sent")
init_recognize(imdata)

while True:
    print("Waiting for Data")