# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import pml
import signalflows

from audio_signal_switch import AudioSignalSwitch

class MultiRenderer( visr.CompositeComponent ):
    def __init__( self, context, name, parent,
                 loudspeakerConfigs,
                 numberOfInputs,
                 numberOfOutputs,
                 interpolationPeriod,
                 diffusionFilters,
                 trackingConfiguration='',
                 controlDataType = pml.UnsignedInteger ):
        super(MultiRenderer,self).__init__( context, name, parent )
        self.input = visr.AudioInputFloat( "in", self, numberOfInputs )
        self.output = visr.AudioOutputFloat( "out", self, numberOfOutputs )
        self.objectInput = visr.ParameterInput("objectIn", self,
                                               protocolType=pml.DoubleBufferingProtocol.staticType,
                                               parameterType=pml.ObjectVector.staticType,
                                               parameterConfig=pml.EmptyParameterConfig() )
        self.controlInput = visr.ParameterInput( "controlIn", self,
                                                protocolType=pml.MessageQueueProtocol.staticType,
                                                parameterType=controlDataType.staticType,
                                                parameterConfig=pml.EmptyParameterConfig() )

        numRenderers = len( loudspeakerConfigs )
        self.outputSwitch = AudioSignalSwitch( context, "OutputSwitch", self,
                                         numberOfChannels=numberOfOutputs,
                                         numberOfInputs=numRenderers,
                                         controlDataType = controlDataType
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
