import math
import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
import cv2
import time
import numpy as np

BaseOptions = mp.tasks.BaseOptions
HandLandmarker = mp.tasks.vision.HandLandmarker
HandLandmarkerOptions = mp.tasks.vision.HandLandmarkerOptions
HandLandmarkerResult = mp.tasks.vision.HandLandmarkerResult
VisionRunningMode = mp.tasks.vision.RunningMode

class hand_landmark_recognizer:
    
    def __init__(self, is_async=False):
        self.model_path = "model/hand_landmarker.task"
        self.hand = hand()
        if is_async:
            self.options_async = HandLandmarkerOptions(base_options=BaseOptions(model_asset_path = self.model_path), running_mode=VisionRunningMode.LIVE_STREAM, result_callback=recognize_result)
            self.detector_async = vision.HandLandmarker.create_from_options(self.options_img)
        else:
            self.options_img = HandLandmarkerOptions(base_options=BaseOptions(model_asset_path = self.model_path), running_mode=VisionRunningMode.IMAGE)
            self.detector_img = vision.HandLandmarker.create_from_options(self.options_img)
        self.isAsync = is_async
    
    def recognize(self, image_data, is_async=False):
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=image_data)
        currentTime = int(time.time_ns()*1000000)
        if self.isAsync:
            self.detector_async.detect_async(image_data, currentTime)
        else:
            result = self.detector_img.detect(mp_image)
            handedness = result.handedness
            for i in range(0, len(handedness)):
                hand_data = handedness[i]
                self.hand = hand(hand_data[0].display_name, result.hand_world_landmarks[i], result.hand_landmarks[i])
            self.draw_result(image_data)
    
    def recognize_result(self, result: HandLandmarkerResult, output_image: mp.Image, timestamp_ms: int):
        cv2.imwrite("Camera_Image.png", output_image)
        cv2.imshow("Image Processed", output_image)
        
    def draw_result(self, image_data):
        img_w = image_data.shape[1]
        img_h = image_data.shape[0]
        disp_img = cv2.cvtColor(image_data, cv2.COLOR_RGB2BGR)
        for landmark in self.hand.getFingerLandmarks(2):
            point_x = int(img_w*landmark.x)
            point_y = int(img_h*landmark.y)
            cv2.circle(disp_img,(point_x, point_y), 2, (0,255,0),-1)
        cv2.imshow("Image", disp_img)
        cv2.waitKey(20000)

class hand:
    
    def __init__(self, handedness = "", landmark = [], normalized_landmark = []):
        self.handedness = handedness
        self.landmark = landmark
        self.normalizedLandmark = normalized_landmark
        self.fingerState = []
        self.setFingerState()
        print(self.isPalmOpen())
    
    def setHandedness(self, handedness):
        self.handedness = handedness
    
    def setLandmark(self, landmark = [], normalized_landmark = []):
        self.landmark = landmark
        self.normalizedLandmark = normalized_landmark
    
    def setFingerState(self):
        if self.handedness != "":
            for i in range(1,6):
                self.fingerState.append(self.isFingerOpen(i))
        print(self.fingerState)
    
    def getLandmark(self):
        return self.normalizedLandmark
    
    def getFingerLandmarks(self, finger, ranks = [1, 2, 3, 4]):
        fingerLandmarks = []
        for rank in ranks:
            fingerLandmarks.append(self.normalizedLandmark[finger*4-4+rank])
        return fingerLandmarks

    def isFingerOpen(self, finger):
        fingerLandmarks = self.getFingerLandmarks(finger,[1,2,4])
        rootLandmark = self.normalizedLandmark[0]
        rootToFingerVector = np.array([rootLandmark.x-fingerLandmarks[0].x, rootLandmark.y-fingerLandmarks[0].y])
        fingerToNextVector = np.array([fingerLandmarks[0].x-fingerLandmarks[1].x, fingerLandmarks[0].y-fingerLandmarks[1].y])
        fingerToTipVector = np.array([fingerLandmarks[0].x-fingerLandmarks[2].x, fingerLandmarks[0].y-fingerLandmarks[2].y])
        if (angle(fingerToTipVector, rootToFingerVector) > 1.57/2 and angle(rootToFingerVector,fingerToNextVector) > 1.57/2):
            return False
        else:
            return True
        
    def pointDirection(self):
        if self.isFingerOpen(2):
            return self.getFingerLandmarks(2,[4])
        
    def isPalmOpen(self):
        for i in range(0,4):
            if not(self.fingerState):
                return False
        for i in range(1,3):
            firstFingerVector = np.array([self.normalizedLandmark[i*4+2].x - self.normalizedLandmark[i*4+1].x, self.normalizedLandmark[i*4+2].y - self.normalizedLandmark[i*4+1].y])
            secondFingerVector = np.array([self.normalizedLandmark[i*4+6].x - self.normalizedLandmark[i*4+5].x, self.normalizedLandmark[i*4+6].y - self.normalizedLandmark[i*4+5].y])
            if (angle(firstFingerVector, secondFingerVector)<1.57/18):
                return False
        return True
                
    
    

def angle(x,y):
    norm_x = np.linalg.norm(x)
    norm_y = np.linalg.norm(y)
    return math.acos(np.dot(x,y)/(norm_x*norm_y))
