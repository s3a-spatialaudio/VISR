#!/usr/bin/env python 

import argparse
import sys
import socket

parser = argparse.ArgumentParser()

# The positional (mandatory) arguments:
parser.add_argument( "objectid", help="The object id, an integer >= 0." )
parser.add_argument( "direction", help="The direction of arrival of the plane wave, given in degrees as 'azimuth:elevation' 'azimuth' (which implies elevation=0)" )

parser.add_argument( "-v", "--verbosity", action="count", default=0, help="Print the network message after sending." )
parser.add_argument( "-r", "--receiver", default="localhost:8888", help="The destination network address, format: host:port, default: localhost:8888." )
parser.add_argument( "-l", "--level", default=1.0, help="Sound level of the source, linear scale 0..1.0, default: 1.0." )
parser.add_argument( "-d", "--referencedistance", default="0.0", help="Reference distance [in meters], used to calculate the time delay of arrival (if the rendering algorithm supports this). Default: 0.0." )
parser.add_argument( "-p", "--priority", default="0", help="Rendering priority, integer >= 0, default: 0." );
parser.add_argument( "-c", "--channelid", default="-1", help="Channel id, denotes the audio channel for the source signal. Integer >=0. Default: -1 meaning that the object id is used as channel id.");
parser.add_argument( "-g", "--groupid", default="0", help="Group id, integer >= 0." );

args = parser.parse_args()

destStrs = args.receiver.split(":")
destHost = destStrs[0]
destPort = int( destStrs[1] )

objectId = int(args.objectid)

pos = args.direction.split(":")
azimuth = float(pos[0])
if len( pos ) == 2:
  elevation = float(pos[1])
else:
  elevation = 0.0

if int(args.channelid) < 0:
  channelId = objectId
else:
  channelId = int(args.channelid)

groupId = int(args.groupid)

priority = int(args.priority)

level = float( args.level )

refdist = float( args.referencedistance )

msg = "{ \"objects\":[{\"id\": %d, \"channels\": %d,\n\"type\": \"plane\", \"group\": %d, \"priority\": %d, \"level\": %f, \"direction\": {\"az\": %f, \"el\": %f, \"refdist\": %f } }]  }" % ( objectId, channelId, groupId, priority, level, azimuth, elevation, refdist )

udpSocket = socket.socket(socket.AF_INET, # Internet
                          socket.SOCK_DGRAM) # UDP

udpSocket.sendto( msg, (destHost, destPort) )

if args.verbosity >= 1:
  print( "Send message %s to %s:%d." % ( msg, destHost, destPort ) )

