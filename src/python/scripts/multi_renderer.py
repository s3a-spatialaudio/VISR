# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import rcl
import pml
import signalflows

# Usage in debugger:
# exec(open("./pythonAtomicFlow.py").read())

import numpy as np
import matplotlib.pyplot as plt

class OutputSwitch( visr.AtomicComponent ):
    def __init__( self, context, name, parent,
                 numberOfChannels,
                 numberOfInputs,
                 initialInput=0 ):
        super( OutputSwitch, self ).__init__( context, name, parent )
        self.inputs = []
        for inIdx in range(0, numberOfInputs):
            portName = "in_%d" % inIdx
            self.inputs.append( visr.AudioInputFloat( portName, self, numberOfChannels ) )

        self.output = visr.AudioOutputFloat( "out", self, numberOfChannels )
        self.controlInput = visr.ParameterInput( "controlIn", self, 
                                                protocolType=pml.MessageQueueProtocol.staticType,
                                                parameterType=pml.UnsignedInteger.staticType,
                                                parameterConfig=pml.EmptyParameterConfig() )
        self.activeInput = initialInput
        
    def process( self ):
        protocolIn = self.controlInput.protocolInput()
        if not protocolIn.empty():
            while not protocolIn.empty():
                newInput = protocolIn.front()
                protocolIn.pop()
            if newInput >= len(self.inputs):
                raise IndexError("Control input exceeds number of ports")
            self.activeInput = newInput
        sig = self.inputs[self.activeInput].data()
        self.output.set( sig )

class MultiRenderer( visr.CompositeComponent ):
    def __init__( self, context, name, parent,
                 loudspeakerConfigs,
                 numberOfInputs,
                 numberOfOutputs, 
                 interpolationPeriod,
                 diffusionFilters,
                 trackingConfiguration='' ):
        super(MultiRenderer,self).__init__( context, name, parent )
        self.input = visr.AudioInputFloat( "in", self, numberOfInputs )
        self.output = visr.AudioOutputFloat( "out", self, numberOfOutputs )
        self.objectInput = visr.ParameterInput("objectIn", self,
                                               protocolType=pml.DoubleBufferingProtocol.staticType,
                                               parameterType=pml.ObjectVector.staticType,
                                               parameterConfig=pml.EmptyParameterConfig() )
        self.controlInput = visr.ParameterInput( "controlIn", self, 
                                                protocolType=pml.MessageQueueProtocol.staticType,
                                                parameterType=pml.UnsignedInteger.staticType,
                                                parameterConfig=pml.EmptyParameterConfig() )

        numRenderers = len( loudspeakerConfigs )
        self.outputSwitch = OutputSwitch( context, "OutputSwitch", self, 
                                         numberOfChannels=numberOfOutputs,
                                         numberOfInputs=numRenderers
                                         )
        self.parameterConnection( self.controlInput, self.outputSwitch.parameterPort('controlIn') )
        self.audioConnection( self.outputSwitch.audioPort('out'), self.output )
        
        self.renderers = []
        for rendererIdx in range(0,numRenderers ):
            rendererName =  "renderer%d" % rendererIdx
            config = loudspeakerConfigs[rendererIdx]
            
            decorrFilters = pml.MatrixParameterFloat( diffusionFilters[0: config.numberOfRegularLoudspeakers,: ] )
            
            renderer = signalflows.CoreRenderer( context,rendererName,self,
                                                loudspeakerConfiguration=config,
                                                numberOfInputs=numberOfInputs,
                                                numberOfOutputs=numberOfOutputs, 
                                                interpolationPeriod=interpolationPeriod, 
                                                diffusionFilters=decorrFilters,
                                                trackingConfiguration=trackingConfiguration )
            self.audioConnection( self.input, renderer.audioPort('audioIn') )
            self.audioConnection( renderer.audioPort('audioOut'), self.outputSwitch.audioPort( 'in_%d' % rendererIdx ) )
            self.parameterConnection( self.objectInput, renderer.parameterPort('objectDataInput'))
            self.renderers.append( renderer )
