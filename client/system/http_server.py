import sys
import socket
import client.system.log as log
from io import StringIO
from random import uniform

app = None

class bgl_http_server:
    def __init__(self, application, http_host, http_port):
        global app
        app = application
        HOST = http_host
        PORT = http_port
        self.connected = False
        self.connection = None
        self.commandBuffer = ""
        self.buffer = StringIO()

        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setblocking(0)
        log.write(log.INFO, "{0}: Socket created".format(self))
        self.socket.bind( (HOST, PORT) ) 
        log.write(log.INFO, "{0}: Socket bound".format(self))
        self.socket.listen(10)
        self.message = ""
        self.buffer = StringIO()

    def process(self):
        http_request = self.message
        self.message = ""  
        action = http_request.split("GET ")[1].split(" HTTP")[0]
        sys.stdout = self.buffer
       
        response = "HELLLO DOGGIES" 
        print("HTTP/1.1 200 OK")
        print("")
        print(response.encode("UTF-8"))
        sys.stdout = sys.__stdout__
        self.connection.sendall(bytes(str(self.buffer.getvalue()) + "\r\n","UTF-8"))

        print(http_request)

    def tick(self):
        if not self.connection:
            try:
                conn, addr = self.socket.accept()
                conn.setblocking(0)
                self.connection = conn
            except:
                self.connection = None

        if self.connection:
            try:
                self.message = self.message + self.connection.recv(128).decode('UTF-8')
            except:
                self.process()
                self.connection = None


    def __del__(self):
        self.socket.close()

