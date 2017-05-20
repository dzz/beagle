import sys
import socket
import client.system.log as log
from io import StringIO
from random import uniform
import urllib.parse
import json

class bgl_http_server:
    def __init__(self, application, http_host, http_port, api_key):
        self.application = application
        HOST = http_host
        PORT = http_port
        self.connected = False
        self.connection = None
        self.commandBuffer = ""
        self.api_key = api_key

        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        self.socket.setblocking(0)
        log.write(log.INFO, "{0}: Socket created".format(self))
        self.socket.bind( (HOST, PORT) ) 
        log.write(log.INFO, "{0}: Socket bound".format(self))
        self.port = http_port
        self.socket.listen(10)
        self.message = ""
        self.buffer = StringIO()

    def process(self):
        output_buffer = StringIO()
        http_request = self.message
        self.message = ""  
        index = False
        action = ""
        try:
            url = http_request.split("GET ")[1].split(" HTTP")[0]
            action = urllib.parse.unquote(url)[1:]
        except:
            index = True
        if action == "":
            index = True

        if index:
            try:
                response = self.application.http_serve_index()
            except NameError:
                response = "THIS APPLICATION DOES NOT SERVE HTTP INDEX"
                pass
        else:
            try:
                decoded = json.loads(action)
                try:
                    request_key = decoded['api_key']
                    if(request_key != self.api_key):
                        response = "AUTHENTICATION ERROR"
                    else:
                        response = self.application.http_route_json(decoded)
                except NameError:
                    response = "THIS APPLICATION DOES NOT RESPOND TO JSON"
                    pass
            except:
                response = "TRY BETTER JSON"

        
        print("formatting response:{0}".format(response))
        sys.stdout = output_buffer
        print("HTTP/1.1 200 OK")
        if index:
            print("Content-Type: text/html; charset=utf-8")
        else:
            print("Content-Type: application/json")
        print("")
        print(response)
        sys.stdout = sys.__stdout__
        self.connection.sendall(bytes(str(output_buffer.getvalue()),"UTF-8"))




    def tick(self):
        if not self.connection:
            try:
                conn, addr = self.socket.accept()
                conn.setblocking(0)
                conn.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
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

