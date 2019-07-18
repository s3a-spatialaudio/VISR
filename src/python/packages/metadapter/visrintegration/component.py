# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 12:25:59 2016

@author: af5u13
"""

from metadapter import Engine

# VISR core packages.
import visr
import pml

import json

class Component( visr.AtomicComponent ):
    """
    Main object to process the metadata.

    """
    def __init__( self, context, name, parent,
                 processorConfig,
                 objectVectorInput = True,  # receive input as pml.ObjectVector parameters (as opposed to JSON)
                 objectVectorOutput = True, # send output as pml.ObjectVector parameters (as opposed to JSON)
                 oscControlPort = False,
                 jsonControlPort = False,
                 alwaysProcess = True,
                 verbose = False):
        """ Construction function. """
        super( Component, self ).__init__( context, name, parent )
        if objectVectorInput:
            self.textInput = False
            self.objectInput = visr.ParameterInput( "objectIn", self,
                                                   pml.ObjectVector.staticType,
                                                   pml.DoubleBufferingProtocol.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.textInput = True
            self.objectInput = visr.ParameterInput( "objectIn", self,
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueueProtocol.staticType,
                                                   pml.EmptyParameterConfig() )
        if objectVectorOutput:
            self.textOutput = False
            self.objectOutput = visr.ParameterOutput( "objectOut", self,
                                                   pml.ObjectVector.staticType,
                                                   pml.DoubleBufferingProtocol.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.textOutput = True
            self.objectOutput = visr.ParameterOutput( "objectOut", self,
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueueProtocol.staticType,
                                                   pml.EmptyParameterConfig() )
        if oscControlPort:
            self.oscControlInput = visr.ParameterInput( "oscControlIn", self,
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueueProtocol.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.oscControlInput = None
        if jsonControlPort:
            self.jsonControlInput = visr.ParameterInput( "jsonControlIn", self,
                                                   pml.StringParameter.staticType,
                                                   pml.MessageQueueProtocol.staticType,
                                                   pml.EmptyParameterConfig() )
        else:
            self.jsonControlInput = None
        self._engine = Engine( processorConfig, alwaysProcess, verbose )

    def process( self ):
        objectMessages = []
        if self.textInput:
            while not self.objectInput.protocol.empty():
                msg = self.objectInput.protocol.front()
                objectMessages.append( msg.str )
                self.objectInput.protocol.pop()
        elif self.objectInput.protocol.changed():
            ov = self.objectInput.protocol.data()
            objectMessages.append( self._decodeObjectVectorFromParameter( ov ) )
            self.objectInput.protocol.resetChanged()
        oscControlMessages = []
        if self.oscControlInput is not None:
            while not self.oscControlInput.protocol.empty():
                oscControlMessages.append( self.oscControlInput.protocol.front().bytes )
                self.oscControlInput.protocol.pop()
        jsonControlMessages = []
        if self.jsonControlInput is not None:
            while not self.jsonControlInput.protocol.empty():
                payload = self.jsonControlInput.protocol.front().str
                jsonControlMessages.append( payload )
                self.jsonControlInput.protocol.pop()
        newObjectVector = self._engine.process( objectMessages,
                                                oscControlMessages,
                                                jsonControlMessages )
        fullJsonMsg = self._encodeObjectVectorFromJson( newObjectVector )
        if self.textOutput:
            self.objectOutput.protocol.enqueue( pml.StringParameter(fullJsonMsg) )
        else:
            outProtocol = self.objectOutput.protocol
            ov = outProtocol.data()
            ov.clear()
            ov.updateFromJson( fullJsonMsg )
            outProtocol.swapBuffers()

    # %% =========================================================================
    # Private methods

    def _decodeObjectVectorFromJson( self, jsonMessage ):
        """ Decode a JSON message into a Python vector of objects
            TODO: Add logic to combine object vectors (optionally)
        """
        vec = json.loads( jsonMessage )
        self.objectVector = vec['objects']

    def _decodeObjectVectorFromParameter( self, objectVector ):
        jsonMessage = objectVector.writeJson()
        return jsonMessage

    def _encodeObjectVectorFromJson( self, objectVector ):
        """ Encode an audio object vector into a JSON string,
            including the enclosing '{"objects": }' layer
            TODO: Does not need to be class method.
        """
        fullMsgObj = { 'objects': objectVector }
        msg = json.dumps( fullMsgObj ) + '\n'
        return msg

