#!/usr/bin/env python

# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:52:45 2016

@author: af5u13
"""

from metadapter.networked import Engine

import argparse
import os
import sys

# Command line parsing
cmdLineParser = argparse.ArgumentParser()

cmdLineParser.add_argument( "-c", "--configFile", default="config.xml", help="XML configuration file containing the processing chain.")
cmdLineParser.add_argument( "-r", "--receivePort", default=4243, help="UDP port number for receiving scene data." )
cmdLineParser.add_argument( "--controlPort", default=None, help="UDP port number for control messages." )
cmdLineParser.add_argument( "-s", "--sendHost", default=None, help="Network address and UDP port to which the processed scene data is sent.")
cmdLineParser.add_argument( "-p", "--sceneSendInterval", default=0.1, help="Send interval for outgoing scene data vectors")
cmdLineParser.add_argument( "-v", "--verbosity", default=0, help="Verbosity level. Integer number from 0 (no verbosity) to N (max. verbosity).")
cmdLineParser.add_argument( "-t", "--objectTimeout", default=86400, help="Object timeout [in seconds].")

#cmdLineParser.add_argument( "--splitSceneVector", default=false, help="Split the scene vector if the message length is too long")

# Define the maximum length of the input scene data.
# The default (8192) is slightly too small for larger SAW scenes.
# A too small value results in no packages received at all.
inputPacketSize = 65536

# Maximum length for sent data.
# The system default is normally about 8192 bytes, which can be too small for us. Large values might not be portable.
# TODO: Consider making this an optional command line parameter.
outputPacketSize = 65536

arguments = cmdLineParser.parse_args()
configFileRaw = arguments.configFile
configFile = os.path.abspath( configFileRaw )

receivePort = int(arguments.receivePort)
if arguments.controlPort == None:
    controlPort = None
else:
    controlPort = int(arguments.controlPort)

sceneSendInterval = float(arguments.sceneSendInterval)
verbosity = int(arguments.verbosity)

if arguments.sendHost == None:
    sendHostArgs = []
else:
    sendHostArgs = arguments.sendHost.split(":")
if len( sendHostArgs ) == 0:
    sendHostPort = 4242
    sendHostName = "127.0.0.1"
elif len( sendHostArgs ) == 1:
    sendHostPort = int(sendHostArgs[0])
    sendHostName = "127.0.0.1"
else:
    sendHostPort = int(sendHostArgs[1])
    sendHostName = sendHostArgs[0]

objectTimeout = float(arguments.objectTimeout)

try:
    engine = Engine( configFile,
                    sendHost=sendHostName,
                    sendPort=sendHostPort,
                    receivePort=receivePort,
                    controlDataPort=controlPort,
                    sendInterval=sceneSendInterval,
                    inputPacketSize=inputPacketSize,
                    outputPacketSize=outputPacketSize,
                    verbose=verbosity,
                    objectTimeoutSeconds=objectTimeout )

    print( 'starting metadata adaptation engine...' )
    sys.stdout.flush()
    engine.run()
    print( 'Metadata adaptation engine stopped.' )
    sys.stdout.flush()
except BaseException as ex:
    print( "Caught exception \"%s\" while instantiation and running the metadata processing engine: %s" %
           (ex.__class__, str(ex) ) )
