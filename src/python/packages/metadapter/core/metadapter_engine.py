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
    from metadapter.thirdparty.OSC import decodeOSC

from copy import deepcopy
import json
import os.path
from collections import namedtuple
from time import clock

import xml.etree.ElementTree as et

from .instantiate_subflow import instantiateSubFlow

class Engine():
    """
    Core medadata adaptation engine.
    It can be used standalone, but mostly as the internal component of some other processing component.

    """
    ObjectRecord = namedtuple( 'ObjectRecord', [ 'timeStamp', 'objectDesc' ] )

    def __init__( self,
                 processorConfig,
                 alwaysProcess = True,
                 verbose = False,
                 objectTimeoutSeconds = None ):
        """ Construction function. """
        self.verbose = verbose
        self._objectDict = {}
        # self._objectVector = []
        self._initializeProcessing( processorConfig )
        self._alwaysProcess = alwaysProcess
        self._objectTimeout = objectTimeoutSeconds

    def process( self, objectMetadata, oscControlData, jsonControlData = None ):
        """
        Main processing function, called periodically.

        Parameters
        ==========
           objectMetadata:
               A list of JSON strings containing updates to object vector.
           oscControlData:
               List of byte arrays containing OSC messages to control processors.
           jsonControlData:
               List of JSON strings to control processors.
        """

        # Prune expired objects if a timeout is specified.
        if self._objectTimeout is not None:
            deadline = clock() - self._objectTimeout
            removeIds = [k for k,v in self._objectDict.items() if v.timeStamp < deadline ]
            for k in removeIds:
                del self._objectDict[k]
        for message in objectMetadata:
            self._applyObjectMetadata( message )
        # Copy the payload of the remaining entries into the object vector,
        objectVector = [deepcopy(rec.objectDesc) for id,rec in self._objectDict.items() ]

        for message in oscControlData:
            self._applyOscControlData( message )
        for message in jsonControlData:
            self._applyJsonControlData( message )

        if self._alwaysProcess or len(objectMetadata) > 0 or len(oscControlData) > 0:
            return self._executeProcessingChain( objectVector )
        else:
            return None

    # %% =========================================================================
    # Private methods

    def _initializeProcessing( self, processorConfig ):
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
        self._processorLookup = {}
        topLevelNodes = list(configRoot)
        self._processingSequence = instantiateSubFlow( topLevelNodes, self._processorLookup, self.verbose )
        # For the moment, continue.

    def _applyObjectMetadata( self, jsonData ):
        """ Callback function that is called when data has been received through the UDP input port. """
        if self.verbose:
            print( 'metadapter::Engine:datagramReceived(): %d bytes received' % len(jsonData) )
        newObjects = self._decodeObjectVectorFromJson( jsonData )
        timeStamp = clock()
        for obj in newObjects:
            objId = obj["id"]
            self._objectDict[objId] = Engine.ObjectRecord( timeStamp, obj )

    def _applyOscControlData( self, oscMsg ):
        """ Callback function that is called when data is received on the control data port"""
        try:
            msg = decodeOSC( oscMsg )
            msgAddress = msg[0]
            msgPayload = msg[2:]
            # This will fail with OSC bundles (which need some recursive handling)
            addressParts = msgAddress[1:].split(b'/') # Avoid a split at the leading '/'
            if len( addressParts ) < 2:
                print( "Received OSC message does not fit the \"/processor/parameter\" scheme." )
            processorName = addressParts[0].decode()
            paramNameList = addressParts[1:]
            paramName = '/'.join( [x.decode() for x in paramNameList] ) # Transform byte arrays->strings
            self._transmitControlData( processorName, paramName, msgPayload )
        except BaseException as ex:
            print( 'MetadataAdaptationEngine.controlDataReceived(): Parsing exception: %s'
                % str(ex) )

    def _applyJsonControlData( self, jsonMsg ):
        """ Callback function that is called when data is received on the control data port"""
        try:
            decodedMessage = json.loads( jsonMsg )
            self._applyFullJsonMessage( decodedMessage )
        except BaseException as ex:
            print( 'MetadataAdaptationEngine.controlDataReceived(): Parsing exception: %s'
                % str(ex) )

    def _transmitControlData( self, processorName, parameterKey, value ):
        """ """
        try:
            processor = self._processorLookup[ processorName ]
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
        return vec['objects']

    def _executeProcessingChain( self, objectVector ):
        if self._processingSequence == None:
            return objectVector
        else:
            return self._processingSequence.processObjectVector( deepcopy(objectVector) )

    def _applyFullJsonMessage( self, message ):
        """
        Apply a full JSON control message, consisting processor names, one or more keyword arguments, ad a list of parameter for each keyword argument.
        """
        if isinstance( message, list ):
            for elem in message:
                self._applyFullJsonMessage( elem )
        elif isinstance( message, dict ):
            for proc, arguments in message.items():
                self._applyProcessorJsonMessages( proc, arguments )
        else:
            raise ValueError("At the moment, only lists and dictionaries are supported as full JSON messages.")

    def _applyProcessorJsonMessages( self, processorName, messages ):
        """
        Dispatch one or more key-value messages to a given processor.
        """
        if isinstance( messages, list ):
            for elem in messages:
                self._applyProcessorJsonMessages( processorName, elem )
        elif isinstance( messages, dict ):
            for key, parameters in messages.items():
                self._applySingleJsonMessage( processorName, key, parameters )
        else:
            raise ValueError("At the moment, only lists and dictionaries are supported as key-value sequences.")

    def _applySingleJsonMessage( self, processorName, keyName, message ):
        """ Dispatch a single decoded JSON message of the form "key": [val1, val2, valN] to the
            addressed processor
        """
        self._transmitControlData( processorName, keyName, message )

