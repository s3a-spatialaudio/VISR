# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import pml

class AudioSignalSwitch( visr.AtomicComponent ):
    def __init__( self, context, name, parent,
                 numberOfChannels,
                 numberOfInputs,
                 initialInput=0,
                 controlDataType = pml.UnsignedInteger # Data type
                 ):
        super( AudioSignalSwitch, self ).__init__( context, name, parent )
        self.inputs = []
        for inIdx in range(0, numberOfInputs):
            portName = "in_%d" % inIdx
            self.inputs.append( visr.AudioInputFloat( portName, self, numberOfChannels ) )

        self.output = visr.AudioOutputFloat( "out", self, numberOfChannels )
        self.controlInput = visr.ParameterInput( "controlIn", self,
                                                protocolType=pml.MessageQueueProtocol.staticType,
                                                parameterType=controlDataType.staticType,
                                                parameterConfig=pml.EmptyParameterConfig() )
        self.activeInput = initialInput

    def process( self ):
        protocolIn = self.controlInput.protocolInput()
        if not protocolIn.empty():
            while not protocolIn.empty():
                newInput = int(protocolIn.front().value)
                protocolIn.pop()
            if newInput >= len(self.inputs):
                raise IndexError("Control input exceeds number of ports")
            if self.activeInput != newInput:
                self.activeInput = newInput
                # print( "OutputSwitch: Switched to output #%i." % newInput )
        sig = self.inputs[self.activeInput].data()
        self.output.set( sig )
