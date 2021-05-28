import socket
import time

from .BaseSink import BaseSink


class UdpSink(BaseSink):
    def __init__(self, module, server, port):
        super().__init__(module, server, port)
        
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def trace(self, level, module, frame, text):
        m = super().pack(level, module, frame, text)
        
        self.socket.sendto(m, (self.server, self.port))


class TcpSink(BaseSink):
    def __init__(self, module, server, port):
        super().__init__(module, server, port)
        
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connected = False
        self.lastConnectAttempt = None
        self.__connect()     

    def __connect(self):
        if self.connected:
            return True

        now = time.time()

        if (not self.lastConnectAttempt) or (now - self.lastConnectAttempt > 5):
            self.lastConnectAttempt = now

            try:
                self.socket.connect((self.server, self.port))
                self.connected = True
            except:
                print ("Failed to connect to " + self.server + ":" + str(self.port))
                return False

            return True

        return False    

    def trace(self, level, module, frame, text):
        if not self.__connect():
            return

        m = super().pack(level, module, frame, text)
        self.socket.send(m)
                      

