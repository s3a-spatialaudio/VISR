#!/usr/bin/env python 

import argparse
import sys
import socket

parser = argparse.ArgumentParser()

# The positional (mandatory) arguments:
parser.add_argument( "objectid", help="The object id, an integer >= 0." )
parser.add_argument( "position", help="The object position in Cartesian coordinates, specified as x:y:z or x:y." )

parser.add_argument( "-v", "--verbosity", action="count", default=0, help="Print the network message after sending." )
parser.add_argument( "-r", "--receiver", default="localhost:4242", help="The destination network address, format: host:port, default: localhost:4242." )
parser.add_argument( "-l", "--level", default=1.0, help="Sound level of the source, linear scale 0..1.0, default: 1.0." )
parser.add_argument( "-p", "--priority", default="0", help="Rendering priority, integer >= 0, default: 0." );
parser.add_argument( "-c", "--channelid", default="-1", help="Channel id, denotes the audio channel for the source signal. Integer >=0. Default: -1 meaning that the object id is used as channel id.");
parser.add_argument( "-g", "--groupid", default="0", help="Group id, integer >= 0." );
parser.add_argument( "-d", "--diffuseness", default="0.0", help="Diffuseness, floating point value 0 <= d <= 1" );

args = parser.parse_args()

destStrs = args.receiver.split(":")
destHost = destStrs[0]
destPort = int( destStrs[1] )

objectId = int(args.objectid)

pos = args.position.split(":")
posX = float(pos[0])
posY = float(pos[1])
if len( pos ) == 3:
  posZ = float(pos[2])
else:
  posZ = 0.0

if int(args.channelid) < 0:
  channelId = objectId
else:
  channelId = int(args.channelid)

groupId = int(args.groupid)

priority = int(args.priority)

level = float( args.level )

diffuseness = float( args.diffuseness )

if (diffuseness == 0.0):
  msg = "{ \"objects\":[{\"id\": %d, \"channels\": %d,\n\"type\": \"point\", \"group\": %d, \"priority\": %d, \"level\": %f, \"position\": {\"x\": %f, \"y\": %f, \"z\": %f } }]  }" % ( objectId, channelId, groupId, priority, level, posX, posY, posZ )
else:
  msg = "{ \"objects\":[{\"id\": %d, \"channels\": %d,\n\"type\": \"pointdiffuse\", \"group\": %d, \"priority\": %d, \"level\": %f, \"diffuseness\": %f, \"position\": {\"x\": %f, \"y\": %f, \"z\": %f } }]  }" % ( objectId, channelId, groupId, priority, level, diffuseness, posX, posY, posZ )


udpSocket = socket.socket(socket.AF_INET, # Internet
                          socket.SOCK_DGRAM) # UDP

udpSocket.sendto( msg, (destHost, destPort) )

if args.verbosity >= 1:
  print( "Send message %s to %s:%d." % ( msg, destHost, destPort ) )

