import sys

from Trace import Trace
from Trace.NetSink import TcpSink, UdpSink


def main(args):
    tcp = False
    host = "localhost"
    port = 665

    i = 0; 
    for a in args:
        if a == "-tcp":
            tcp = True
        elif a == "-port":
            if i + 1 < len(args):
                port = int(args[i + 1])
        elif a == "-server":
            if i + 1 < len(args):
                host = args[i + 1]

        i += 1

    if not tcp:
        u = UdpSink("Run", host, port)
    else:
        u = TcpSink("Run", host, port)

    u.debug("Debug message")
    u.info("Info message")
    u.warning("Warning message")
    u.error("Error message")
    u.highest("Highest message")


if __name__ == "__main__":
    main(sys.argv)