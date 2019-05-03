#!/usr/bin/env python 

import argparse
import sys
import socket

def translatePosition( x, y, z ):
  """ Translate the position into the coordinate system hard-coded into the current position decoder.""" 
  return x, y, z

parser = argparse.ArgumentParser()

# The positional (mandatory) arguments:

parser.add_argument( "position", help="The object position in Cartesian coordinates, specified as x:y:z or x:y." )
parser.add_argument( "-v", "--verbosity", action="count", default=0, help="Print the network message after sending." )
parser.add_argument( "-r", "--receiver", default="localhost:8888", help="The destination network address, format: host:port, default: localhost:8888." )

args = parser.parse_args()

destStrs = args.receiver.split(":")
destHost = destStrs[0]
destPort = int( destStrs[1] )

pos = args.position.split(":")
posX = float(pos[0])
posY = float(pos[1])
if len( pos ) == 3:
  posZ = float(pos[2])
else:
  posZ = 0.0

xTrans, yTrans, zTrans = translatePosition( posX, posY, posZ )

nTime = 0
faceId = 0
  
msg = "{ \"nTime\": %d, \"iFace\": %d, \"headJoint\": { \"X\": %f, \"Y\": %f, \"Z\": %f } }" % ( nTime, faceId, xTrans, yTrans, zTrans )

msgBuffer = msg.encode('ascii')

udpSocket = socket.socket(socket.AF_INET, # Internet
                          socket.SOCK_DGRAM) # UDP

udpSocket.sendto( msgBuffer, (destHost, destPort) )

if True: # args.verbosity >= 1:
  print( "Send message %s to %s:%d." % ( msg, destHost, destPort ) )

