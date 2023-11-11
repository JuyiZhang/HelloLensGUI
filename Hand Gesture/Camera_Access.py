import struct
from picamera2 import Picamera2
import socket
import numpy as np

picam2 = Picamera2()
video_config = picam2.create_video_configuration({"format":"RGB888","size": (640,480)})
picam2.video_configuration.controls.FrameRate = 25.0
picam2.configure(video_config)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("0.0.0.0", 10001))
    sock.listen
    
    conn, addr = sock.accept()
    data = conn.recv(640*480*24)
    rgbarray = np.frombuffer(data, np.uint8).reshape(640,480,3)
    print(rgbarray)
    