# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 12:25:59 2016

@author: af5u13
"""

from sys import version_info

if version_info.major <= 2:
    from OSC import decodeOSC
else:
    # Use the self-made port for Python 3 (experimental)
    from thirdparty.OSC import decodeOSC

from copy import deepcopy
from twisted.internet.protocol import DatagramProtocol
from twisted.internet.task import LoopingCall
from twisted.internet import reactor

import socket
import time
from collections import namedtuple
import sys

from metadapter import Engine

# TODO: Consider moving into separate file (not as a nested class then)
class NetworkUdpReceiver( DatagramProtocol ):
    """ Internal class to handle receit of network messages """
    def __init__( self, parent, verbose=False ):
        self.parent = parent
        self.verbose = verbose

    def startProtocol( self ):
        if self.verbose:
            print( 'NetworkUdpReceiver.startProtocol()' )
        # Nothing really to do here (for a receiver)
    def datagramReceived( self, data, address ):
        self.parent.datagramReceived( data, address )

class ControlDataUdpReceiver( DatagramProtocol ):
    """ Internal class to handle receipt of network messages """
    def __init__( self, parent, verbose=False ):
        self.parent = parent
        self.verbose = verbose

    def startProtocol( self ):
        if self.verbose:
            print( 'ControlDataUdpReceiver.startProtocol()' )
        # Nothing really to do here (for a receiver)
    def datagramReceived( self, data, address ):
        self.parent.controlDataReceived( data, address )


class NetworkUdpSender( DatagramProtocol ):
    """ Internal class to handle receit of network messages """
    def __init__( self, parent, address, port, verbose=False, sendBufferSize=None ):
        self.parent = parent
        self.sendAddress = address
        self.sendPort = port
        self.sendBufferSize = sendBufferSize
        self.verbose = verbose
    def startProtocol( self ):
        if self.verbose:
            print( 'NetworkUdpSender.startProtocol()' )
        if self.sendBufferSize != None:
            self.transport.socket.setsockopt( socket.SOL_SOCKET, socket.SO_SNDBUF, self.sendBufferSize )
        self.transport.connect( self.sendAddress, self.sendPort )

    def stopProtocol(self):
        #on disconnect
        print( "NetworkUdpSender: disconnected, attempting reconnect." )
        reactor.listenUDP(0, self)

    def sendData( self, data ):
        if self.transport == None:
            print( "Skipped sending scene data: No UDP transport present." )
            return 0
        else:
#            try:
            return self.transport.write( data )
#            except BaseException as ex:
#                return Failure( ex, ex.__class__ )

# Tuple data type to store objects
ObjectRecord = namedtuple( 'ObjectRecord', [ 'timeStamp', 'objectDesc' ] )

class MetadataAdaptationEngine:
    """
    Main object to process the metadata.

    """

    def __init__( self, processorConfig, receiveHost='127.0.0.1', receivePort=4241,
                  sendHost='127.0.0.1', sendPort=4242, sendInterval=0.05,
                  verbose=False,
                  controlDataPort=None,
                  inputPacketSize=None,
                  outputPacketSize=None,
                  objectTimeoutSeconds = 86400.0 ):
        """ Construction function. """
        self.initializeNetwork( receivePort, sendHost, sendPort, sendInterval,
                                controlDataPort, inputPacketSize, outputPacketSize = outputPacketSize )
        self._objectDict = {}
        self._objectTimeOutSec = float(objectTimeoutSeconds)
        self._oscControlMessages = []
        self._engine = Engine( processorConfig, alwaysProcess=True, verbose=verbose )

    def run( self ):
        reactor.run()

    def initializeNetwork( self, receivePort, sendAddress, sendPort, sendInterval,
                           controlDataPort=None,
                           inputPacketSize=None,
                           outputPacketSize=None):
        """Network initialization function"""
        self.udpReceiver = NetworkUdpReceiver( self, self.verbose )
        self.udpSender = NetworkUdpSender( self, sendAddress, sendPort, sendBufferSize = outputPacketSize )
        if inputPacketSize == None:
            reactor.listenUDP( receivePort, self.udpReceiver )
        else:
            reactor.listenUDP( receivePort, self.udpReceiver, maxPacketSize=inputPacketSize )
        # The sender does not listen but this is required to instantiate a transport object
        # within the NetworkUdpSender object.
        reactor.listenUDP( 0, self.udpSender )
        self.sendTask = LoopingCall( self.periodicTrigger )
        triggerDeferred = self.sendTask.start( sendInterval, now=False )
        triggerDeferred.addErrback( self.triggerError )
        if controlDataPort != None:
            self.controlDataPort = ControlDataUdpReceiver( self, self.verbose )
            reactor.listenUDP( controlDataPort, self.controlDataPort )
        else:
            self.controlDataPort = None
        self.maxPacketSize=outputPacketSize

    def datagramReceived(self, data, address ):
        """ Callback function that is called when data has been received through the UDP input port. """
        if self.verbose:
            print( 'MetadataAdaptationEngine:datagramReceived(): %d bytes received' % len(data) )
        tmpObjects = self.decodeObjectVector( data )
        timeStamp = time.clock()
        for obj in tmpObjects:
            objId = obj["id"]
            self.objectDict[objId] = ObjectRecord( timeStamp, obj )

    def dispatchOSC( msg ):
        """ """


    def controlDataReceived( self, data, address ):
        """ Callback function that is called when data is received on the control data port"""
        # TODO: We could use pythonosc.dgram_is_message() to check whether data is an OSC message.
        try:
            msg = decodeOSC( data )
            msgAddress = bytes.decode(msg[0])
            msgPayload = (msg[2:])

            # This will fail with OSC bundles (which need some recursive handling)
            addressParts = msgAddress[1:].split('/') # Avoid a split at the leading '/'

            if len( addressParts ) < 2:
                print( "Received OSC message does not fit the \"/processor/parameter\" scheme." )
            self._dispatchControlData( processorName, paramName, msgPayload )
        except BaseException as ex:
            print( 'MetadataAdaptationEngine.controlDataReceived(): Parsing exception: %s'
                % str(ex) )

    def dispatchControlData( self, processorName, parameterKey, value ):
        """ """
        self._oscControlMessages.append( processorName, parameterKey, value )

    def periodicTrigger( self ):
        """ Periodically called function to perform the metadata processing and to send the generated scene vector. """
        # Todo: Check whether previous calculation has finished. Otherwise, defer execution
        try:
            deadline = time.clock() - self.objectTimeOutSec
            # First prune expired objects
            removeIds = [k for k,v in self.objectDict.items() if v.timeStamp < deadline ]
            for k in removeIds:
                del self.objectDict[k]
            # Copy the payload of the remaining entries into the object vector,
            objVec = [deepcopy(rec.objectDesc) for id,rec in self.objectDict.items() ]

            # TODO: Take care of mutual accesses to the object vectors.
            # self.secondaryObjectVector = deepcopy( self.primaryObjectVector )
            processedObjectVector = self.executeProcessingChain( objVec )
            jsonMsg = bytearray( self.encodeObjectVector( processedObjectVector ), 'ascii' )

            if len(jsonMsg) > self.maxPacketSize:
              print( "splitting the object vector" )
              # hack: send the object vector n objects at a time
              nSend=3
              nObjects=len(processedObjectVector)
              sendStart=0
              sentObjects=0
              while sentObjects<nObjects:
                #print ("\n")
                jsonMsg = bytearray( self.encodeObjectVector( processedObjectVector[sendStart:sendStart+nSend] ), 'ascii' )
                #print( "message length: %d" % len(jsonMsg ) )
                try:
                    numTrans = self.udpSender.sendData( jsonMsg )
                except BaseException as ex:
                    print( "Failed sending object vector :%s" % str(ex) )
                    raise ex
                #print( "Number of bytes transmitted at first attempt %s" % str(numTrans) )
                if numTrans != len(jsonMsg):
                  #print( "sendMessage: Difference between message length (%d) and transmitted bytes (%s)" % ( len(jsonMsg), str(numTrans) ))
                  numTrans = self.udpSender.sendData( jsonMsg )
                  #print( "Number of bytes transmitted at second attempt %s" % str(numTrans) )
                  if numTrans != len(jsonMsg):
                    print( "sendMessage: Difference between message length (%d) and transmitted bytes (%s) after 2nd attempt" % ( len(jsonMsg), str(numTrans) ))


                sendStart=sendStart+nSend
                sentObjects=sentObjects+nSend
                #time.sleep(0.1)



            else:
              #print( "message length: %d" % len(jsonMsg ) )
              numTrans = self.udpSender.sendData( jsonMsg )
              if numTrans != len(jsonMsg):
                print( "sendMessage: Difference between message length (%d) and transmitted bytes (%s)" % ( len(jsonMsg), str(numTrans) ))
        except BaseException as ex:
            print( "Failed sending object vector :%s" % str(ex) )
            raise ex

    def triggerError( reason ):
        """ Callback method to be called when the periodicTrigger() method fails."""
        print( "Periodic trigger caused an error: %s." % str(reason) )
        sys.exit()
