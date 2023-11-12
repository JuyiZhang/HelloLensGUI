import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
import cv2
import time

model_path = "model/hand_landmarker.task"
BaseOptions = mp.tasks.BaseOptions
HandLandmarker = mp.tasks.vision.HandLandmarker
HandLandmarkerOptions = mp.tasks.vision.HandLandmarkerOptions
HandLandmarkerResult = mp.tasks.vision.HandLandmarkerResult
VisionRunningMode = mp.tasks.vision.RunningMode



def init_recognize(image_data):
    mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=image_data)
    currentTime = int(time.time())
    with HandLandmarker.create_from_options(options) as landmarker:
        result = landmarker.detect(mp_image)
        
    
def recognize_result(result: HandLandmarkerResult, output_image: mp.Image, timestamp_ms: int):
    cv2.imwrite("Camera_Image.png", output_image)
    cv2.imshow("Image Processed", output_image)
    print(result)
    
options = HandLandmarkerOptions(base_options=BaseOptions(model_asset_path = model_path), running_mode=VisionRunningMode.LIVE_STREAM)#, result_callback=recognize_result)
detector = vision.HandLandmarker.create_from_options(options)