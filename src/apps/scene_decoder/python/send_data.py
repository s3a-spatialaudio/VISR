#!/usr/bin/env python 

import sys
import socket

def usage():
  print( "Usage: %s <receiver host name> <receiver port> <file name>" )


# print( str(sys.argv[1:]) )
if( len( sys.argv ) != 4 ):
  usage()
  exit();

recvHost = sys.argv[1]
recvPortStr = sys.argv[2]
fileName = sys.argv[3]

msg = "{ \"objects\":[{\"id\": 1, \"type\": \"point\", \"group\": 0, \"priority\": 3, \"level\": 0.8, \"position\": {\"x\": 2.4, \"y\": 1.2, \"z\": -0.3 } }, {\"id\": 5, \"type\": \"point\", \"group\": 1, \"priority\": 2, \"level\": 0.375, \"position\": {\"x\": 1.5, \"y\": -1.5, \"z\": 0 } }] }"

# print( msg )

recvPort = int(recvPortStr)
udpSocket = socket.socket(socket.AF_INET, # Internet
                          socket.SOCK_DGRAM) # UDP

udpSocket.sendto( msg, (recvHost, recvPort) )
