import sys
import socket
import client.system.log as log
from io import StringIO

app = None

class telnet_console:
    def __init__(self, application, telnet_host, telnet_port):
        global app
        app = application
        HOST = telnet_host
        PORT = telnet_port
        self.connected = False
        self.connection = None
        self.commandBuffer = ""
        self.buffer = StringIO()

        self.socket = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        self.socket.setblocking(0)
        log.write(log.INFO, "PYTEL: Socket created")
        self.socket.bind( (HOST, PORT) ) 
        log.write(log.INFO, "PYTEL: Socket bound")
        self.socket.listen(10)

    def tick(self):
        if not self.connected:
            try:
                conn, addr = self.socket.accept()
                conn.setblocking(0)
            except:
                conn = None

            if conn is not None:
                log.write(log.INFO,"PYTEL: Connected: [{0}]".format(addr))
                self.connection = conn
                self.connection.send(bytes("Beagle - Interactive Debug Server 0.1.1\r\n--------------------\r\n","UTF-8"))
                self.connected = True
        else:
            try:
                data = self.connection.recv(1024).decode("UTF-8")
                if(data==""):
                    self.connected = False

                try:
                    compiled = compile( data, "remote","exec" )
                except Exception as e:
                    self.connection.sendall(bytes(str(e),"UTF-8"))

                try:
                    sys.stdout = self.buffer
                    exec(compiled)
                    sys.stdout = sys.__stdout__
                    self.connection.sendall(bytes(str(self.buffer.getvalue()) + "\r\n","UTF-8"))
                    self.buffer = StringIO()
                except Exception as e:
                    self.connection.sendall(bytes(str(e) + "\r\n","UTF-8"))

            except:
                pass


    def __del__(self):
        self.socket.close()


        
