import socket 
from http.server import HTTPServer, BaseHTTPRequestHandler 
import logging 
import RPi.GPIO as GPIO 
import subprocess


DEBUG = False
PORT = 15003
class requestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # if GPIO.input(26) == 0:
        #     print("flare trigger")
        # print(p32.getState())
        try:

            self.send_response(200)
            self.send_header('content-type', 'text/html')
            self.end_headers()
            request = self.path[1:] 
            if 'screen_reboot' in request:
                #call sudo reboot command
                subprocess.run(["sudo", "reboot"])

            
            # else:
            #     self.wfile.write(b"Relay 1 is OFF")
        except IOError: 
            self.send_error(500, "Server Error") 
 
def getLocalIP(): 
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
    s.connect(("8.8.8.8", 80)) 
    ip = s.getsockname()[0]
    s.close()
    return ip

def main():
    print("Attempting Connection")
    print(getLocalIP())
    server_address = (getLocalIP(), PORT)
    server = HTTPServer(server_address, requestHandler)
    print("Server running")
    # subprocess.run(["python", "exitSoundService.py"])
    server.serve_forever()


if __name__ == '__main__':
    main()

