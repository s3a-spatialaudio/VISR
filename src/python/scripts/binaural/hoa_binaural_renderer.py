# -*- coding: utf-8 -*-
"""
Created on Mon Oct  9 09:49:40 2017

S3A Binaural toolbox for the VISR framework

Signal flow graph for binaural synthesis based on Higher Order Ambisonics rendering.

@author: af5u13
"""

import visr
import pml

import rcl

from hoa_encoder import HoaEncoder
from hoa_coefficient_rotation import HoaCoefficientRotation
from readSofa import readSofaFile

import numpy as np

class HoaBinauralRenderer( visr.CompositeComponent ):

    def __init__( self,
                 context, name, parent, 
                 numberOfObjects,
                 maxHoaOrder,
                 sofaFile,
                 interpolationSteps = None,
                 headTracking = True,
                 objectChannelAllocation = False
                 ):
        numHoaCoeffs = (maxHoaOrder+1)**2
        
        [pos, filters, delays] = readSofaFile( sofaFile )

        super( HoaBinauralRenderer, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
        self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )

        if interpolationSteps is None:
            interpolationSteps = context.period

        self.objectEncoder = HoaEncoder( context, 'HoaEncoder', self,
                                        numberOfObjects = numberOfObjects, 
                                        hoaOrder = maxHoaOrder, 
                                        channelAllocation = objectChannelAllocation )

        self.parameterConnection( self.objectVectorInput, self.objectEncoder.parameterPort("objectVector"))

        self.encoderMatrix = rcl.GainMatrix( context, "encoderMatrix", self,
                                             numberOfInputs = numberOfObjects,
                                             numberOfOutputs = (maxHoaOrder+1)**2,
                                             interpolationSteps = interpolationSteps,
                                             initialGains = 0.0,
                                             controlInput = True )
        self.audioConnection( self.objectSignalInput, self.encoderMatrix.audioPort("in") )

        filterMtx = np.concatenate( (filters[0:numHoaCoeffs,0,:], filters[0:numHoaCoeffs,1,:]) )

        routings = pml.FilterRoutingList()
        for idx in range(0,numHoaCoeffs):
            routings.addRouting( idx, 0, idx, 1.0 )
            routings.addRouting( idx, 1, idx+numHoaCoeffs, 1.0 )

        self.binauralFilterBank = rcl.FirFilterMatrix( context, 'binauralFilterBank', self,
                                                    numberOfInputs = numHoaCoeffs,
                                                    numberOfOutputs = 2,
                                                    filterLength = filters.shape[-1],
                                                    maxFilters = 2*numHoaCoeffs,
                                                    maxRoutings = 2*numHoaCoeffs,
                                                    filters = filterMtx,
                                                    routings = routings,
                                                    controlInputs=rcl.FirFilterMatrix.ControlPortConfig.NoInputs )

        self.audioConnection( self.encoderMatrix.audioPort("out"), self.binauralFilterBank.audioPort("in") )
        self.audioConnection( self.binauralFilterBank.audioPort("out"), self.binauralOutput )

        if headTracking:
            self.trackingInput = visr.ParameterInput( "headTracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            self.coefficientRotator = HoaCoefficientRotation( context, 'coefficientRotator', self,
                                                              numberOfObjects = numberOfObjects, 
                                                              hoaOrder = maxHoaOrder)
            self.parameterConnection( self.trackingInput, self.coefficientRotator.parameterPort("headTracking") )
            self.parameterConnection( self.objectEncoder.parameterPort("coefficientOutput"),
                                     self.coefficientRotator.parameterPort("coefficientInput") )
            self.parameterConnection( self.coefficientRotator.parameterPort("coefficientOutput"),
                                     self.encoderMatrix.parameterPort("gainInput") )
        else:
            self.parameterConnection( self.objectEncoder.parameterPort("coefficientOutput"),
                                     self.encoderMatrix.parameterPort("gainInput") )
