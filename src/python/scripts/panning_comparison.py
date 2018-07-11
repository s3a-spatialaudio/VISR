# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import rbbl
import rcl
import pml

# Usage in debugger:
# exec(open("./PanningComparison.py").read())

#import numpy as np
#import matplotlib.pyplot as plt

from audio_signal_switch import AudioSignalSwitch
from pointsource_trajectory_generator import PointSourceTrajectoryGenerator

class StandardVbap( visr.CompositeComponent ):
    def __init__(self, context, name, parent,
             numberOfInputs,
             numberOfOutputs,
             arrayConfig,
             interpolationSteps ):
        super( StandardVbap, self ).__init__(context, name, parent )
        self.input = visr.AudioInputFloat( "in", self, numberOfInputs )
        self.output = visr.AudioOutputFloat( "out", self, numberOfOutputs )


        self.objectInput = visr.ParameterInput( "objectVectorInput", self,
                                               parameterType=pml.ObjectVector.staticType,
                                               protocolType=pml.DoubleBufferingProtocol.staticType,
                                               parameterConfig=pml.EmptyParameterConfig() )
        self.panningCalculator = rcl.PanningCalculator( context, "GainCalculator", self,
                                                       arrayConfig=arrayConfig,
                                                       numberOfObjects=numberOfInputs,
                                                       separateLowpassPanning=False )

        self.panningMatrix = rcl.GainMatrix( context, "PanningMatrix", self,
                                             numberOfInputs=numberOfInputs,
                                             numberOfOutputs=numberOfOutputs,
                                             interpolationSteps=interpolationSteps,
                                             initialGains = 0.0,
                                             controlInput=True )

        self.audioConnection( self.input, self.panningMatrix.audioPort("in") )
        self.audioConnection( self.panningMatrix.audioPort("out"), self.output )

        self.parameterConnection( self.objectInput, self.panningCalculator.parameterPort("objectVectorInput") )
        self.parameterConnection( self.panningCalculator.parameterPort("gainOutput"), self.panningMatrix.parameterPort("gainInput") )

class LfHfVbap( visr.CompositeComponent ):
    def __init__(self, context, name, parent,
             numberOfInputs,
             numberOfOutputs,
             arrayConfig,
             interpolationSteps,
             lfFilter = rbbl.BiquadCoefficientFloat( 0.001921697757295, 0.003843395514590,
                                                  0.001921697757295,-1.824651307057289,
                                                  0.832338098086468 ),
             hfFilter = rbbl.BiquadCoefficientFloat( 0.914247351285939, 1.828494702571878,
                                                  -0.914247351285939, -1.824651307057289,
                                                  0.832338098086468 )
             ):
        super( LfHfVbap, self ).__init__(context, name, parent )
        self.input = visr.AudioInputFloat( "in", self, numberOfInputs )
        self.output = visr.AudioOutputFloat( "out", self, numberOfOutputs )

        self.objectInput = visr.ParameterInput( "objectVectorInput", self,
                                               parameterType=pml.ObjectVector.staticType,
                                               protocolType=pml.DoubleBufferingProtocol.staticType,
                                               parameterConfig=pml.EmptyParameterConfig() )

        self.panningCalculator = rcl.PanningCalculator( context, "GainCalculator", self,
                                                       arrayConfig=arrayConfig,
                                                       numberOfObjects=numberOfInputs,
                                                       separateLowpassPanning=True )

        filterMtx = rbbl.BiquadCoefficientMatrixFloat( 2*numberOfInputs, 1 )
        for idx in range(0,numberOfInputs ):
            filterMtx[idx,0] = lfFilter
            filterMtx[idx+numberOfInputs,0] = hfFilter

        self.filterBank = rcl.BiquadIirFilter( context, "filterBank", self,
                                         numberOfChannels=2*numberOfInputs,
                                         numberOfBiquads=1, # TODO: allow variable number of sections.
                                         initialBiquads=filterMtx,
                                         controlInput = False )
        self.audioConnection( self.input,
                             [ i % numberOfInputs for i in range(0, 2*numberOfInputs) ],
                             self.filterBank.audioPort("in"),
                             range(0,2*numberOfInputs))

        self.lfMatrix = rcl.GainMatrix( context, "LfPanningMatrix", self,
                                             numberOfInputs=numberOfInputs,
                                             numberOfOutputs=numberOfOutputs,
                                             interpolationSteps=interpolationSteps,
                                             initialGains = 0.0,
                                             controlInput=True )
        self.audioConnection( self.filterBank.audioPort("out"),
                             range(0,numberOfInputs),
                             self.lfMatrix.audioPort("in"),
                             range(0,numberOfInputs))

        self.hfMatrix = rcl.GainMatrix( context, "HfPanningMatrix", self,
                                             numberOfInputs=numberOfInputs,
                                             numberOfOutputs=numberOfOutputs,
                                             interpolationSteps=interpolationSteps,
                                             initialGains = 0.0,
                                             controlInput=True )
        self.audioConnection( self.filterBank.audioPort("out"),
                             range(numberOfInputs, 2*numberOfInputs),
                             self.hfMatrix.audioPort("in"),
                             range(0,numberOfInputs))

        self.signalMix = rcl.Add( context, "SignalMix", self,
                                 numInputs=2, width = numberOfOutputs )
        self.audioConnection( self.signalMix.audioPort("out"), self.output )

        self.audioConnection( self.lfMatrix.audioPort("out"), self.signalMix.audioPort("in0") )
        self.audioConnection( self.hfMatrix.audioPort("out"), self.signalMix.audioPort("in1") )


        self.parameterConnection( self.objectInput, self.panningCalculator.parameterPort("objectVectorInput") )
        self.parameterConnection( self.panningCalculator.parameterPort("lowFrequencyGainOutput"), self.lfMatrix.parameterPort("gainInput") )
        self.parameterConnection( self.panningCalculator.parameterPort("gainOutput"), self.hfMatrix.parameterPort("gainInput") )

class PanningComparison( visr.CompositeComponent ):
    def __init__( self, context, name, parent,
                 loudspeakerConfig,
                 numberOfInputs,
                 numberOfOutputs,
                 interpolationSteps = None,
                 controlDataType = pml.Float ):
        super(PanningComparison,self).__init__( context, name, parent )

        if interpolationSteps is None:
            interpolationSteps = context.period

        numberOfRenderers = 2

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

        self.signalSwitch = AudioSignalSwitch( context, "OutputSwitch", self,
                                         numberOfChannels=numberOfOutputs,
                                         numberOfInputs=numberOfRenderers,
                                         controlDataType=controlDataType
                                         )
        self.parameterConnection( self.controlInput, self.signalSwitch.parameterPort('controlIn') )
        self.audioConnection( self.signalSwitch.audioPort('out'), self.output )

        self. standardRenderer = StandardVbap( context, "StandardVbap",self,
                                              arrayConfig=loudspeakerConfig,
                                              numberOfInputs=numberOfInputs,
                                              numberOfOutputs=numberOfOutputs,
                                              interpolationSteps=interpolationSteps )

        self. lfHfRenderer = LfHfVbap( context, "LfHfVbap",self,
                                              arrayConfig=loudspeakerConfig,
                                              numberOfInputs=numberOfInputs,
                                              numberOfOutputs=numberOfOutputs,
                                              interpolationSteps=interpolationSteps )
                                              # Use default filters for the moment
        self.audioConnection( self.input, self.standardRenderer.audioPort('in') )
        self.audioConnection( self.standardRenderer.audioPort('out'), self.signalSwitch.audioPort( 'in_0' ) )
        self.parameterConnection( self.objectInput, self.standardRenderer.parameterPort('objectVectorInput'))

        self.audioConnection( self.input, self.lfHfRenderer.audioPort('in') )
        self.audioConnection( self.lfHfRenderer.audioPort('out'), self.signalSwitch.audioPort( 'in_1' ) )
        self.parameterConnection( self.objectInput, self.lfHfRenderer.parameterPort('objectVectorInput'))

class RealTimePanningComparisonTrajectory( visr.CompositeComponent ):
    def __init__( self, context, name, parent,
                 loudspeakerConfig,
                 numberOfInputs,
                 numberOfOutputs,
                 trajectoryPositions,
                 trajectoryUpdateSamples = None,
                 sourceLevel=1.0,
                 sourceObjectId = 0,
                 controlDataType = pml.Float,
                 controlReceivePort=8889,
                 ):

        if trajectoryUpdateSamples is None:
            trajectoryUpdateSamples = context.period

        super(RealTimePanningComparisonTrajectory,self).__init__( context, name, parent)
        self.input = visr.AudioInputFloat( "in", self, numberOfInputs )
        self.output = visr.AudioOutputFloat( "out", self, numberOfOutputs )

        self.multiRenderer = PanningComparison(context, "MultiRenderer", self,
                                           loudspeakerConfig=loudspeakerConfig,
                                           numberOfInputs=numberOfInputs,
                                           numberOfOutputs=numberOfOutputs,
                                           interpolationSteps=trajectoryUpdateSamples,
                                           controlDataType=pml.Float)
        self.audioConnection( self.input, self.multiRenderer.audioPort("in" ) )
        self.audioConnection( self.multiRenderer.audioPort("out" ), self.output )

        self.sceneGenerator = PointSourceTrajectoryGenerator( context, "SceneDecoder", self,
                 positions=trajectoryPositions,
                 updateRateSamples = trajectoryUpdateSamples,
                 objectId = sourceObjectId,
                 groupId = 0,
                 priority = 0,
                 objectLevel = sourceLevel )

        self.parameterConnection( self.sceneGenerator.parameterPort( "objectVectorOutput"),
                                 self.multiRenderer.parameterPort( "objectIn" ) )

        self.controlReceiver = rcl.UdpReceiver( context, "ControlReceiver", self,
                                               port=controlReceivePort,
                                               mode=rcl.UdpReceiver.Mode.Asynchronous)
        self.controlDecoder = rcl.ScalarOscDecoder( context, "ControlDecoder", self )
        self.controlDecoder.setup(dataType='float')
        self.parameterConnection( self.controlReceiver.parameterPort("messageOutput"),
                                 self.controlDecoder.parameterPort("datagramInput") )
        self.parameterConnection( self.controlDecoder.parameterPort( "dataOut"),
                                 self.multiRenderer.parameterPort( "controlIn" ) )
