# -*- coding: utf-8 -*-
"""
Created on Mon Feb 26 19:28:57 2018

@author: andi
"""

import visr
import rcl
import pml

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