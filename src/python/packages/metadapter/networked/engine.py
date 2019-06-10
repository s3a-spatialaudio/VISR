# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 12:25:59 2016

@author: af5u13
"""

from sys import version_info

from copy import deepcopy
from twisted.internet.protocol import DatagramProtocol
from twisted.internet.task import LoopingCall
from twisted.internet import reactor

import socket
import sys
import json

from .. import Engine

# TODO: Consider moving into separate file (not as a nested class then)
class NetworkUdpReceiver( DatagramProtocol ):
    """ Internal class to handle receit of network messages """
    def __init__( self, parent, verbose=False ):
        self.parent = parent
        self._verbose = verbose

    def startProtocol( self ):
        if self._verbose:
            print( 'NetworkUdpReceiver.startProtocol()' )
        # Nothing really to do here (for a receiver)
    def datagramReceived( self, data, address ):
        self.parent.datagramReceived( data, address )

class ControlDataUdpReceiver( DatagramProtocol ):
    """ Internal class to handle receipt of network messages """
    def __init__( self, handlerMethod, verbose=False ):
        if not callable( handlerMethod ):
            raise ValueError( 'The argument "handlerMethod" is not callable.' )
        self._verbose = verbose
        self._handler = handlerMethod

    def startProtocol( self ):
        if self._verbose:
            print( 'ControlDataUdpReceiver.startProtocol()' )
        # Nothing really to do here (for a receiver)
    def datagramReceived( self, data, address ):
        self._handler( data, address )

class NetworkUdpSender( DatagramProtocol ):
    """ Internal class to handle receit of network messages """
    def __init__( self, parent, address, port, verbose=False, sendBufferSize=None ):
        self.parent = parent
        self.sendAddress = address
        self.sendPort = port
        self.sendBufferSize = sendBufferSize
        self._verbose = verbose
    def startProtocol( self ):
        if self._verbose:
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

class MetadataAdaptationEngine:
    """
    Main object to process the metadata.

    """

    def __init__( self, processorConfig, receiveHost='127.0.0.1', receivePort=4241,
                  sendHost='127.0.0.1', sendPort=4242, sendInterval=0.05,
                  verbose=False,
                  oscControlDataPort=None,
                  jsonControlDataPort=None,
                  inputPacketSize=None,
                  outputPacketSize=None,
                  objectTimeoutSeconds = None ):
        """ Construction function. """
        self._verbose=verbose
        self.initializeNetwork( receivePort, sendHost, sendPort, sendInterval,
                                oscControlDataPort = oscControlDataPort,
                                jsonControlDataPort=jsonControlDataPort,
                                inputPacketSize=inputPacketSize,
                                outputPacketSize = outputPacketSize )
        self._objectMessages = []
        self._oscControlMessages = []
        self._jsonControlMessages = []
        self._engine = Engine( processorConfig,
                              alwaysProcess=True,
                              verbose=verbose,
                              objectTimeoutSeconds = objectTimeoutSeconds )

    def run( self ):
        reactor.run()

    def initializeNetwork( self, receivePort, sendAddress, sendPort, sendInterval,
                           oscControlDataPort=None,
                           jsonControlDataPort=None,
                           inputPacketSize=None,
                           outputPacketSize=None):
        """Network initialization function"""
        self._udpReceiver = NetworkUdpReceiver( self, self._verbose )
        self._udpSender = NetworkUdpSender( self, sendAddress, sendPort, sendBufferSize = outputPacketSize )
        if inputPacketSize == None:
            reactor.listenUDP( receivePort, self._udpReceiver )
        else:
            reactor.listenUDP( receivePort, self._udpReceiver, maxPacketSize=inputPacketSize )
        self.maxPacketSize=outputPacketSize

        # The sender does not listen but this is required to instantiate a transport object
        # within the NetworkUdpSender object.
        reactor.listenUDP( 0, self._udpSender )
        self.sendTask = LoopingCall( self.periodicTrigger )
        triggerDeferred = self.sendTask.start( sendInterval, now=False )
        triggerDeferred.addErrback( self.triggerError )
        if oscControlDataPort is None:
            self._oscControlDataReceiver = None
        else:
            self._oscControlDataReceiver = ControlDataUdpReceiver( self._oscControlDataReceived,
                                                                  self._verbose )
            reactor.listenUDP( oscControlDataPort, self._oscControlDataReceiver )
        if jsonControlDataPort is None:
            self._jsonControlDataReceiver = None
        else:
            self._jsonControlDataReceiver = ControlDataUdpReceiver( self._jsonControlDataReceived,
                                                                   self._verbose )
            reactor.listenUDP( jsonControlDataPort, self._jsonControlDataReceiver )

    def datagramReceived(self, data, address ):
        """ Callback function that is called when data has been received through the UDP input port. """
        if self._verbose:
            print( 'MetadataAdaptationEngine:datagramReceived(): %d bytes received' % len(data) )
        # The JSON data needs to be converted to strings
        # from the 'bytes' returned from the network socket.
        self._objectMessages.append( data.decode('utf-8') )

    def _oscControlDataReceived( self, data, address ):
        """ Callback function that is called when data is received on the control data port"""
        # TODO: We could use pythonosc.dgram_is_message() to check whether data is an OSC message.
        try:
            # OSC data must remain as 'bytes' data.
            self._oscControlMessages.append( data )
        except BaseException as ex:
            print( 'MetadataAdaptationEngine.controlDataReceived(): Parsing exception: %s'
                % str(ex) )

    def _jsonControlDataReceived( self, data, address ):
        """ Callback function that is called when data is received on the JSON control data port"""
        try:
            # The JSON data needs to be converted to strings
            # from the 'bytes' returned from the network socket.
            self._jsonControlMessages.append( data.decode('utf-8') )
        except BaseException as ex:
            print( 'MetadataAdaptationEngine.controlDataReceived(): Parsing exception: %s'
                % str(ex) )

    def periodicTrigger( self ):
        """ Periodically called function to perform the metadata processing and to send the generated scene vector. """
        # Todo: Check whether previous calculation has finished. Otherwise, defer execution
        try:
            # TODO: Take care of mutual accesses to the object vectors.
            # self.secondaryObjectVector = deepcopy( self.primaryObjectVector )
            processedObjectVector = self._engine.process( self._objectMessages,
                                                         self._oscControlMessages,
                                                         self._jsonControlMessages )
            self._objectMessages.clear()
            self._oscControlMessages.clear()
            self._jsonControlMessages.clear()

            jsonMsg = bytearray( self._encodeObjectVector( processedObjectVector ), 'ascii' )

            if len(jsonMsg) > self.maxPacketSize:
              print( "splitting the object vector" )
              # hack: send the object vector n objects at a time
              nSend=3
              nObjects=len(processedObjectVector)
              sendStart=0
              sentObjects=0
              while sentObjects<nObjects:
                #print ("\n")
                jsonMsg = bytearray( self._encodeObjectVector( processedObjectVector[sendStart:sendStart+nSend] ), 'ascii' )
                #print( "message length: %d" % len(jsonMsg ) )
                try:
                    numTrans = self._udpSender.sendData( jsonMsg )
                except BaseException as ex:
                    print( "Failed sending object vector :%s" % str(ex) )
                    raise ex
                #print( "Number of bytes transmitted at first attempt %s" % str(numTrans) )
                if numTrans != len(jsonMsg):
                  #print( "sendMessage: Difference between message length (%d) and transmitted bytes (%s)" % ( len(jsonMsg), str(numTrans) ))
                  numTrans = self._udpSender.sendData( jsonMsg )
                  #print( "Number of bytes transmitted at second attempt %s" % str(numTrans) )
                  if numTrans != len(jsonMsg):
                    print( "sendMessage: Difference between message length (%d) and transmitted bytes (%s) after 2nd attempt" % ( len(jsonMsg), str(numTrans) ))


                sendStart=sendStart+nSend
                sentObjects=sentObjects+nSend
                #time.sleep(0.1)


            else:
              #print( "message length: %d" % len(jsonMsg ) )
              numTrans = self._udpSender.sendData( jsonMsg )
              if numTrans != len(jsonMsg):
                print( "sendMessage: Difference between message length (%d) and transmitted bytes (%s)" % ( len(jsonMsg), str(numTrans) ))
        except BaseException as ex:
            print( "Failed sending object vector :%s" % str(ex) )
            raise ex

    def triggerError( reason ):
        """ Callback method to be called when the periodicTrigger() method fails."""
        print( "Periodic trigger caused an error: %s." % str(reason) )
        sys.exit()

    def _encodeObjectVector( self, objectVector ):
        """ Encode an audio object vector into a JSON string,
            including the enclosing '{"objects": }' layer
            TODO: Does not need to be class method.
        """
        fullMsgObj = { 'objects': objectVector }
        msg = json.dumps( fullMsgObj ) + '\n'
        return msg
