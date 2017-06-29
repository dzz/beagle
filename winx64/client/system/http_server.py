import sys
import socket
import client.system.log as log
from io import StringIO
from random import uniform
import urllib.parse
import json
import threading

class bgl_http_server:
    def __init__(self, application, http_host, http_port, api_key):
        self.application = application
        self.running = False
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

    def process(self, message, connection):
        output_buffer = StringIO()
        http_request = message
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
                            response = "[ ~ NO http_route_json ON application.http_handler ~ ]\nhint: make your game inherit from beagle_api.basic_web_app, and assign it to an http_handler variable in your application's main.py"
                except Exception as e:
                    response = "[ TRY BETTER JSON ] {0}".format(e)

            if action.find("/frontend/") == 0:
                try:
                    texthtml = True
                    resource = action.split("frontend/")[1]
                    response = self.application.http_handler.http_route_frontend(resource)
                except:
                    response = "[ ~ NO http_route_frontend ON application.http_handler ~ ]\nhint: make your game inherit from beagle_api.basic_web_app, and assign it to an http_handler variable in your application's main.py"

        out = "";
        out = out + "HTTP/1.1 200 OK\n"
        if texthtml:
            out = out + "Content-Type: text/html; charset=utf-8\n"
        else:
            out = out + "Content-Type: application/json; charset=utf-8\n"
        out = out + "\n"
        out = out + response

        print(out)
        try:
            connection.sendall(bytes(out, 'utf-8'))
        except Exception as e:
            print("WTF?{0}".format(e))




    def readMessage(self,client,addr):
        message = ""
        try:
            while True:
                message = message + client.recv(1024).decode('UTF-8')
        except:
            self.process(message, client)
            client.close()

    def listen(self):
        client, addr = self.socket.accept()
        client.setblocking(0)
        client.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        try:
            print("making thread....")
            messageThread = threading.Thread( target = self.readMessage, args = (client,addr))
            print("new thread {0}".format(messageThread))
            messageThread.start()
        except Exception as e:
            print("{0}".format(e))


    def run_forever(self):
        self.running = True
        while True:
            try:
                self.listen()
            except:
                pass

    def tick(self):
        if not self.running:
            listenThread = threading.Thread( target = self.run_forever )
            listenThread.start()
        else:
            return

        ##if not self.connection:
        ##    try:
        ##        conn, addr = self.socket.accept()
        ##        conn.setblocking(0)
        ##        conn.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        ##        self.connection = conn
        ##    except:
        ##        self.connection = None

        ##if self.connection:
        ##    try:
        ##        self.message = self.message + self.connection.recv(1024).decode('UTF-8')
        ##    except:
        ##        self.process()
        ##        self.connection = None


    def __del__(self):
        self.socket.close()

