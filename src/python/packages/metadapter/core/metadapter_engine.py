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

import xml.etree.ElementTree as et

from .instantiate_subflow import instantiateSubFlow

class Engine():
    """
    Core medadata adaptation engine.
    It can be used standalone, but mostly as the internal component of some other processing component.
    """

    def __init__( self,
                 processorConfig,
                 alwaysProcess = True,
                 verbose = False):
        """ Construction function. """
        self.verbose = verbose
        self._objectVector = []
        self._initializeProcessing( processorConfig )
        self._alwaysProcess = alwaysProcess

    def process( self, objectMetadata, oscControlData ):
        for message in objectMetadata:
            self._applyObjectMetadata( message )
        for message in oscControlData:
            self._applyOscControlData( message )
        if self._alwaysProcess or len(objectMetadata) > 0 or len(oscControlData) > 0:
            return self._executeProcessingChain( self._objectVector )
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
        # TODO:
        self._objectVector = self._decodeObjectVectorFromJson( jsonData )

    def _applyOscControlData( self, oscMsg ):
        """ Callback function that is called when data is received on the control data port"""
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
            self._transmitControlData( processorName, paramName, msgPayload )
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
