#!/usr/bin/env python 

import argparse
import sys
import socket

parser = argparse.ArgumentParser()

# The positional (mandatory) arguments:
parser.add_argument( "objectid", help="The object id, an integer >= 0." )
parser.add_argument( "outputchannels", help="A comma-separated string consisting of the output channel ids" )

parser.add_argument( "-v", "--verbosity", action="count", default=0, help="Print the network message after sending." )
parser.add_argument( "-r", "--receiver", default="localhost:4242", help="The destination network address, format: host:port, default: localhost:4242." )
parser.add_argument( "-l", "--level", default=1.0, help="Sound level of the source, linear scale 0..1.0, default: 1.0." )
parser.add_argument( "-p", "--priority", default="0", help="Rendering priority, integer >= 0, default: 0." );
parser.add_argument( "-c", "--channelid", default="-1", help="Channel id, denotes the audio channels for the source signal. string, either comma-separated 1,2,3 or ranges. Default: -1 meaning that the object id is used as channel id.");
parser.add_argument( "-g", "--groupid", default="0", help="Group id, integer >= 0." );

args = parser.parse_args()

destStrs = args.receiver.split(":")
destHost = destStrs[0]
destPort = int( destStrs[1] )

objectId = int(args.objectid)

if len(args.channelid) <= 0:
  channelId = str(objectId)
else:
  channelId = args.channelid
  
outputchannels = args.outputchannels

groupId = int(args.groupid)

priority = int(args.priority)

level = float( args.level )

msg = "{ \"objects\":[{\"id\": %d, \"outputChannels\": \"%s\", \"channels\": \"%s\", \"type\": \"channel\", \"group\": %d, \"priority\": %d, \"level\": %f}]  }" % ( objectId, outputchannels, channelId, groupId, priority, level )

udpSocket = socket.socket(socket.AF_INET, # Internet
                          socket.SOCK_DGRAM) # UDP

udpSocket.sendto( msg, (destHost, destPort) )

if args.verbosity >= 1:
  print( "Send message %s to %s:%d." % ( msg, destHost, destPort ) )

