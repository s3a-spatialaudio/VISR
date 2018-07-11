# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 12:25:59 2016

@author: af5u13
"""

#from sys import version_info
#
#if version_info.major <= 2:
#    from OSC import decodeOSC
#else:
#    # Use the self-made port for Python 3 (experimental)
#    from thirdparty.OSC import decodeOSC

from localOSC import decodeOSC

import visr
import pml
import objectvector as om

from copy import deepcopy
import json
import os.path
#from twisted.internet.protocol import DatagramProtocol
#from twisted.internet.task import LoopingCall
#from twisted.internet import reactor
#from twisted.python.failure import Failure
import xml.etree.ElementTree as et
#import socket

from instantiate_subflow import instantiateSubFlow

## TODO: Consider moving into separate file (not as a nested class then)
#class NetworkUdpReceiver( DatagramProtocol ):
#    """ Internal class to handle receit of network messages """
#    def __init__( self, parent, verbose=False ):
#        self.parent = parent
#        self.verbose = verbose
#
#    def startProtocol( self ):
#        if self.verbose:
#            print( 'NetworkUdpReceiver.startProtocol()' )
#        # Nothing really to do here (for a receiver)
#    def datagramReceived( self, data, address ):
#        self.parent.datagramReceived( data, address )
#
#class ControlDataUdpReceiver( DatagramProtocol ):
#    """ Internal class to handle receit of network messages """
#    def __init__( self, parent, verbose=False ):
#        self.parent = parent
#        self.verbose = verbose
#
#    def startProtocol( self ):
#        if self.verbose:
#            print( 'ControlDataUdpReceiver.startProtocol()' )
#        # Nothing really to do here (for a receiver)
#    def datagramReceived( self, data, address ):
#        self.parent.controlDataReceived( data, address )
#
#
#class NetworkUdpSender( DatagramProtocol ):
#    """ Internal class to handle receit of network messages """
#    def __init__( self, parent, address, port, verbose=False, sendBufferSize=None ):
#        self.parent = parent
#        self.sendAddress = address
#        self.sendPort = port
#        self.sendBufferSize = sendBufferSize
#        self.verbose = verbose
#    def startProtocol( self ):
#        if self.verbose:
#            print( 'NetworkUdpSender.startProtocol()' )
#        if self.sendBufferSize != None:
#            self.transport.socket.setsockopt( socket.SOL_SOCKET, socket.SO_SNDBUF, self.sendBufferSize )
#        self.transport.connect( self.sendAddress, self.sendPort )
#
#    def stopProtocol(self):
#        #on disconnect
#        print( "NetworkUdpSender: disconnected, attempting reconnect." )
#        reactor.listenUDP(0, self)
#
#    def sendData( self, data ):
#        if self.transport == None:
#            print( "Skipped sending scene data: No UDP transport present." )
#            return 0
#        else:
##            try:
#            return self.transport.write( data )
##            except BaseException as ex:
##                return Failure( ex, ex.__class__ )

class Metadapter( visr.AtomicComponent ):
    """
    Main object to process the metadata.

    """

    def __init__( self, context, name, parent,
                 processorConfig,
                 objectVectorInput = True,  # receive input as pml.ObjectVector parameters (as opposed to JSON)
                 objectVectorOutput = True, # send output as pml.ObjectVector parameters (as opposed to JSON)
                 oscControlPort = False,
                 jsonControlPort = False,
                 alwaysProcess = True ):
        """ Construction function. """
        self.objectVector = []
        self.initializeProcessing( processorConfig )
        self.alwaysProcess
        if objectVectorInput:
            self.textInput = False
            self.objectInput = visr.ParameterInput( "objectIn",
                                                   pml.ObjectVector.staticType,
                                                   pml.DoubleBuffering.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.textInput = True
            self.objectInput = visr.ParameterInput( "objectIn",
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueue.staticType,
                                                   pml.EmptyParameterConfig() )
        if objectVectorInput:
            self.textOutput = False
            self.objectOutput = visr.ParameterInput( "objectOut",
                                                   pml.ObjectVector.staticType,
                                                   pml.DoubleBuffering.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.textOutput = True
            self.objectOutput = visr.ParameterOutput( "objectOut",
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueue.staticType,
                                                   pml.EmptyParameterConfig() )
        if oscControlPort:
            self.oscControlInput = visr.ParameterOutput( "oscControlIn",
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueue.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.oscControlInput = None
        if jsonControlPort:
            self.oscControlInput = visr.ParameterOutput( "jsonControlIn",
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueue.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.jsonControlInput = None

    def process( self ):
        triggerProcessing = self.alwaysProcess
        if self.textInput:
            while not self.objectInput.protocol.empty():
                triggerProcessing = True
                msg = self.objectInput.protocol.front()
                self._decodeObjectVectorFromJson( msg )
                self.objectInput.protocol.pop()
        elif self.objectInput.protocol.changed():
            ov = self.objectInput.protocol.data()
            self._decodeObjectVectorFromParameter( self, ov )
            self.objectInput.protocol.resetChanged()
        if self.oscControlInput is not None:
            while not self.oscControlInput.protocol.empty():
                triggerProcessing = True
                msg = self.oscControlInput.protocol.front()
                self._handleOscControlMessage( msg )
                self.oscControlInput.protocol.pop()

        if not triggerProcessing:
            return

        self._executeProcessingChain( self.objectVector )
        fullJsonMsg = self._encodeObjectVectorFromJson( self.objectVector )
        if self.textOutput:
            self.objectOutput.protocol.enqueue( fullJsonMsg )
        else:
            outProtocol = self.objectOutput.protocol
            ov = outProtocol.data()
            ov.clear()
            ov.updateFromJson( fullJsonMsg )
            outprotocol.swapBuffers()


    def initializeProcessing( self, processorConfig ):
        """ Initialize and setup the different processors.
            processorConfig: XML file containing the configuration
        """
        if not os.path.isfile( processorConfig ):
            raise ValueError( 'The XML configuration file \"%s\" does not exist.'
                              % ( processorConfig ) )
        configTree = et.parse( processorConfig )
        configRoot = configTree.getroot()
        # Backward compatibility: Remove 'metadataadapter' laeter.
        if not configRoot.tag in ['metadataadapter','metadapter']:
            raise ValueError('Invalid root node, must be \"metadapter\" or \"metadataadapter\"' )
        if configRoot.tag == 'metadataadapter':
            print( 'The top-level XML element <metadataadapter> is deprecated and support will be removed in a future release. Use <metadapter> instead.' )
        self.processorLookup = {}
        topLevelNodes = list(configRoot)
        self.processingSequence = instantiateSubFlow( topLevelNodes, self.processorLookup, self.verbose )
        # For the moment, continue.

#    def initializeNetwork( self, receivePort, sendAddress, sendPort, sendInterval,
#                           controlDataPort=None,
#                           inputPacketSize=None,
#                           outputPacketSize=None):
#        """Network initialization function"""
#        self.udpReceiver = NetworkUdpReceiver( self, self.verbose )
#        self.udpSender = NetworkUdpSender( self, sendAddress, sendPort, sendBufferSize = outputPacketSize )
#        if inputPacketSize == None:
#            reactor.listenUDP( receivePort, self.udpReceiver )
#        else:
#            reactor.listenUDP( receivePort, self.udpReceiver, maxPacketSize=inputPacketSize )
#        # The sender does not listen but this is required to instantiate a transport object
#        # within the NetworkUdpSender object.
#        reactor.listenUDP( 0, self.udpSender )
#        self.sendTask = LoopingCall( self.periodicTrigger )
#        self.sendTask.start( sendInterval, now=False )
#        if controlDataPort != None:
#            self.controlDataPort = ControlDataUdpReceiver( self, self.verbose )
#            reactor.listenUDP( controlDataPort, self.controlDataPort )
#        else:
#            self.controlDataPort = None

    # %% =========================================================================
    # Private methods

    def _datagramReceived(self, data, address ):
        """ Callback function that is called when data has been received through the UDP input port. """
        if self.verbose:
            print( 'MetadataAdaptationEngine:datagramReceived(): %d bytes received' % len(data) )
        self.primaryObjectVector = self.decodeObjectVector( data )

    def _handleOscControlMessage( self, oscMsg ):
        # TODO: Implement me
        """ Callback function that is called when data is received on the control data port"""
        # TODO: We could use pythonosc.dgram_is_message() to check whether data is an OSC message.
        try:
            msg = decodeOSC( oscMsg )
            msgAddress = msg[0]
            msgPayload = msg[2:]
            # This will fail with OSC bundles (which need some recursive handling)
            addressParts = msgAddress[1:].split('/') # Avoid a split at the leading '/'
            if len( addressParts ) < 2:
                print( "Received OSC message does not fit the \"/processor/parameter\" scheme." )
            processorName = addressParts[0]
            paramNameList = addressParts[1:]
            paramName = '/'.join( paramNameList )
            self.handleControlData( processorName, paramName, msgPayload )
        except BaseException as ex:
            print( 'MetadataAdaptationEngine.controlDataReceived(): Parsing exception: %s'
                % str(ex) )

    def _handleControlData( self, processorName, parameterKey, value ):
        """ """
        # Todo: Do we need thread synchronisation here?
        try:
            processor = self.processorLookup[ processorName ]
        except KeyError:
            print( 'MetadataAdaptationEngine.handleControlData(): processor %s does not exist'
                % (processorName) )
            return
        try:
            processor.setParameter( parameterKey, value )
        except BaseException as ex:
            print( '%s.setParameter(): failed: %s'
                % (processorName, str(ex) ) )



    def _decodeObjectVectorFromJson( self, jsonMessage ):
        """ Decode a JSON message into a Python vector of objects
            TODO: Add logic to combine object vectors (optionally)
        """
        vec = json.loads( jsonMessage )
        self.objectVector = vec['objects']

    def _decodeObjectVectorFromParameter( self, objectVector ):
        jsonMessage = objectVector.writeJson()
        self._decodeObjectVectorFromJson( jsonMessage )


    def _encodeObjectVectorFromJson( self, objectVector ):
        """ Encode an audio object vector into a JSON string,
            including the enclosing '{"objects": }' layer
            TODO: Does not need to be class method.
        """
        fullMsgObj = { 'objects': objectVector }
        msg = json.dumps( fullMsgObj ) + '\n'
        return msg

    def _executeProcessingChain( self, objectVector ):
        if self.processingSequence == None:
            return objectVector
        else:
            return self.processingSequence.processObjectVector( objectVector )

