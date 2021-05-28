import datetime
from inspect import getframeinfo
import json
import os
import threading

from .Trace import Level


class BaseSink:
    def __init__(self, module, server, port):
        self.module = module
        self.server = server
        self.port = port

    def pack(self, level, module, frame, text):
        dt = datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3] # cut microseconds
        
        j = { 
            "level": level,
            "module":module,
            "pid":os.getpid(),
            "tid":threading.get_ident(), 
            "time":dt,
            "text":text  
        }

        if frame:
            fi = getframeinfo(frame)
            j["file"] = fi.filename
            j["line"] = fi.lineno

        s = json.dumps(j)
        u = s.encode('utf-8')
        # first DWORD is JSON length
        l = len(u)
        a = l.to_bytes(4, byteorder='little')

        return a + u    

    def debug(self, text):
        self.trace(Level.Debug, self.module, None, text)  

    def info(self, text):
        self.trace(Level.Info, self.module, None, text)   

    def warning(self, text):
        self.trace(Level.Warning, self.module, None, text)   

    def error(self, text):
        self.trace(Level.Error, self.module, None, text)   

    def highest(self, text):
        self.trace(Level.Highest, self.module, None, text) 

