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
        texthtml = False
        action = ""
        response = "[ ~ BEAGLE, DEFAULT RESPONSE ~ ]".format(self)
        try:
            action = http_request.split("GET ")[1].split(" HTTP")[0]
        except:
            index = True
        if action == "" or action=="/":
            index = True
        if index:
            texthtml = True
            try:
                response = self.application.http_handler.http_serve_index()
            except NameError:
                response = "[ ~ NO INDEX ~ ]"
        else:
            if action.find("/api/") == 0:
                try:
                    urlencoded_json = action.split("/api/")[1][1:]
                    json_string = urllib.parse.unquote(urlencoded_json)
                    decoded = json.loads(json_string);
                    if not 'api_key' in decoded:
                        response = "[ ~ NO api_key IN REQUEST ~ ]"
                    else:
                        try:
                            request_key = decoded['api_key']
                            if(request_key != self.api_key):
                                response = "[ ~ api_key mismatch ~ ]"
                            else:
                                response = json.dumps(self.application.http_handler.http_route_json(decoded))
                        except NameError:
                            response = "[ ~ NO http_route_json ON application.http_handler ~ ]\n" +
                                       "    hint: make your game inherit from beagle_api.basic_web_app, and assign it to an http_handler variable in your application's main.py"
                except:
                    response = "[ TRY BETTER JSON ]"

            if action.find("/frontend/") == 0:
                try:
                    texthtml = True
                    resource = action.split("frontend/")[1]
                    response = self.application.http_handler.http_route_frontend(resource)
                except:
                    response = "[ ~ NO http_route_frontend ON application.http_handler ~ ]\n" +
                               "    hint: make your game inherit from beagle_api.basic_web_app, and assign it to an http_handler variable in your application's main.py"

        sys.stdout = output_buffer
        print("HTTP/1.1 200 OK")
        if texthtml:
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

