# -*- coding: utf-8 -*-
"""
Created on Wed Dec 09 11:36:40 2015

@author: af5u13
"""

from twisted.internet.protocol import DatagramProtocol
from twisted.internet.task import LoopingCall
from twisted.internet import reactor
from twisted.names import client

import json

import sys

receiveAddress = "127.0.0.1"
sendAddress = "152.78.242.119"
sendHostName = "uos-13872.clients.soton.ac.uk"

receivePort = 4242
sendPort = 4241

def queryResult(res=""):
    print( "Address query returned: %s" % str(res) )
    
def queryError(res=""):
    print( "Address query failed: %s" % str(res) )

sendAddressQuery = client.getHostByName( sendHostName )
sendAddressQuery.addCallback( queryResult )
sendAddressQuery.addErrback( queryError )

# Global variables and functions
# TODO: Encapsulate them into objects
processedObjectVector = { "objects": [] }

def setProcessedObjectVector( vec ):
    global processedObjectVector
    processedObjectVector = vec
    
def getProcessedObjectVector():
    global processedObjectVector
    return processedObjectVector

class ObjVecSender(DatagramProtocol):
    def __init__( self, sendAddress, sendPort ):
        self.__sendPort = sendPort
        self.__sendAddress = sendAddress
        # self.transport.connect(sendAddress, sendPort)
    def startProtocol(self):
        print( "ObjVecSender::startProtocol() called")
        self.transport.connect(self.__sendAddress, self.__sendPort )
    def sendData( self, data ):
        # print( "ObjVecSender.sendData() called: % s" % data )
        # print( str( self.transport ))
        self.transport.write( data )        

sendSocket = ObjVecSender( sendAddress, sendPort )

# The function triggered by the send timer
def sendFunction():
    global finalObjectVector
    global sendSocket
    # Do we need to make it threadsafe?
    objVec = getProcessedObjectVector()
    msg = json.dumps( objVec )
    # print( "sendFunction %s" % msg )
    sendSocket.sendData( msg )

sendTask = LoopingCall( sendFunction )
reactor.listenUDP( 0, sendSocket )
sendTask.start(0.1, now=False )

class ObjVecRepeater(DatagramProtocol):
    def __init__( self, sendAddress, sendPort ):
#        # DatagramProtocol.__init__(self)
        self.__sendPort = sendPort
        self.__sendAddress = sendAddress
        
    def datagramReceived(self, data, (host, port)):
        print "Received datagram '%s' from %s:%d." % (data, host, port)
        objVec = json.loads( data )
        # print "Send %r" % (data)
        # self.transport.write( b, (self.__sendAddress, self.__sendPort))
        setProcessedObjectVector( objVec )

reactor.listenUDP(receivePort, ObjVecRepeater( sendAddress, sendPort ))
# reactor.listenUDP(receivePort, ObjVecRepeater())

print( "Reactor starting." )
sys.stdout.flush()
reactor.run()
print( "Reactor finished." )



#==============================================================================
# while True:
#     c = raw_input( "Reactor started. Press 'q'<Return> to terminate.")
#     if c == 'q':
#         break
#     
# print( "Attempting to stop reactor." )
# reactor.stop()
# print( "Reactor stopped, terminating." )
#==============================================================================
