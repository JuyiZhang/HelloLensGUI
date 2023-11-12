from picamera2 import Picamera2

picam2 = Picamera2()
video_config = picam2.create_preview_configuration({"format":"RGB888","size": (640,480)})
picam2.video_configuration.controls.FrameRate = 5.0
picam2.configure(video_config)
picam2.start()

while True:
    im = picam2.capture_array()
    
    