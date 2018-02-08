# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

# exec(open("/home/andi/dev/visr/src/python/scripts/instantiate_multi_renderer.py").read())

import visr
import panning
import rcl
import pml

import numpy as np

# from multi_renderer import OutputSwitch
from multi_renderer import MultiRenderer

class RealTimeMultiRenderer( visr.CompositeComponent ):
    def __init__( self, context, name, parent,
                 loudspeakerConfigFiles,
                 numberOfInputs,
                 numberOfOutputs,
                 interpolationPeriod,
                 diffusionFilterFile,
                 udpReceivePort=8888,
                 controlReceivePort=8889,
                 trackingConfiguration='' ):
        super(RealTimeMultiRenderer,self).__init__( context, name, parent)
        self.input = visr.AudioInputFloat( "in", self, numberOfInputs )
        self.output = visr.AudioOutputFloat( "out", self, numberOfOutputs )

        rendererConfigs = []
        for cfgFile in loudspeakerConfigFiles:
            rendererConfigs.append( panning.LoudspeakerArray(cfgFile) )

        diffFilters = np.array(pml.MatrixParameterFloat.fromAudioFile( diffusionFilterFile ))

        self.multiRenderer = MultiRenderer(context, name, self,
                                           loudspeakerConfigs=rendererConfigs,
                                           numberOfInputs=numberOfInputs,
                                           numberOfOutputs=numberOfOutputs,
                                           interpolationPeriod=interpolationPeriod,
                                           diffusionFilters=diffFilters,
                                           trackingConfiguration='',
                                           controlDataType=pml.Float )
        self.audioConnection( self.input, self.multiRenderer.audioPort("in" ) )
        self.audioConnection( self.multiRenderer.audioPort("out" ), self.output )

        self.sceneReceiver = rcl.UdpReceiver( context, "SceneReceiver", self,
                                             port=udpReceivePort,
                                             mode=rcl.UdpReceiver.Mode.Asynchronous )
        self.sceneDecoder = rcl.SceneDecoder( context, "SceneDecoder", self )
        self.parameterConnection( self.sceneReceiver.parameterPort("messageOutput"),
                                 self.sceneDecoder.parameterPort("datagramInput") )
        self.parameterConnection( self.sceneDecoder.parameterPort( "objectVectorOutput"),
                                 self.multiRenderer.parameterPort( "objectIn" ) )

        self.controlReceiver = rcl.UdpReceiver( context, "ControlReceiver", self,
                                               port=controlReceivePort,
                                               mode=rcl.UdpReceiver.Mode.Asynchronous)
        self.controlDecoder = rcl.ScalarOscDecoder( context, "ControlDecoder", dataType='float')
        self.parameterConnection( self.controlReceiver.parameterPort("messageOutput"),
                                 self.controlDecoder.parameterPort("datagramInput") )
        self.parameterConnection( self.controlDecoder.parameterPort( "dataOut"),
                                 self.multiRenderer.parameterPort( "controlIn" ) )
