# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import numpy as np

# Core VISR packages
import visr
import rbbl
import pml
import rcl

from visr_bst.hoa_components import HoaObjectEncoder
from visr_bst.hoa_components import HoaCoefficientRotation
from visr_bst.util import readSofaFile

class HoaObjectToBinauralRenderer( visr.CompositeComponent ):
    """
    Component to render binaural audio from plane wave and point source objects using an Higher Order Ambisonics (HOA)
    algorithm.
    """
    def __init__( self,
                 context, name, parent,
                 numberOfObjects,
                 maxHoaOrder = None,
                 sofaFile = None,
                 decodingFilters = None,
                 interpolationSteps = None,
                 headOrientation = None,
                 headTracking = True,
                 objectChannelAllocation = False,
                 fftImplementation = 'default'
                 ):
        """
        Constructor.

        Parameters
        ----------
        context : visr.SignalFlowContext
            Standard visr.Component construction argument, holds the block size and the sampling frequency
        name : string
            Name of the component, Standard visr.Component construction argument
        parent : visr.CompositeComponent
            Containing component if there is one, None if this is a top-level component of the signal flow.
        numberOfObjects : int
            The number of audio objects to be rendered.
        maxHoaOrder: int or None
            The maximum HOA order that can be reproduced. If None, the HOA order is deduced
            from the first dimension of the HOA filters (possibly contained in a SOFA file).
        sofaFile: string or NoneType
        decodingFilters : numpy.ndarray or NoneType
            Alternative way to provide the HOA decoding filters.
        interpolationSteps: int
        headOrientation : array-like
            Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
            or the initial view direction
        headTracking: bool
            Whether dynamic head tracking is active.
        objectChannelAllocation: bool
            Whether the processing resources are allocated from a pool of resources
            (True), or whether fixed processing resources statically tied to the audio signal channels are used.
            Not implemented at the moment, so leave the default value (False).
        fftImplementation: string, optional
            The FFT library to be used in the filtering. THe default uses VISR's
            default implementation for the present platform.
        """
        if (decodingFilters is None) == (sofaFile is None ):
            raise ValueError( "HoaObjectToBinauralRenderer: Either 'decodingFilters' or 'sofaFile' must be provided." )
        if sofaFile is None:
            filters = decodingFilters
        else:
            # pos and delays are not used here.
            [pos, filters, delays] = readSofaFile( sofaFile )

        if maxHoaOrder is None:
            numHoaCoeffs = filters.shape[0]
            orderP1 = int(np.floor(np.sqrt(numHoaCoeffs)))
            if orderP1**2 != numHoaCoeffs:
                raise ValueError( "If maxHoaOrder is not given, the number of HOA filters must be a square number" )
            maxHoaOrder = orderP1 - 1
        else:
            numHoaCoeffs = (maxHoaOrder+1)**2

        if filters.ndim != 3 or filters.shape[1] != 2 or filters.shape[0] < numHoaCoeffs:
            raise ValueError( "HoaObjectToBinauralRenderer: the filter data must be a 3D matrix where the second dimension is 2 and the first dimension is equal or larger than (maxHoaOrder+1)^2." )

        super( HoaObjectToBinauralRenderer, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
        self.objectVectorInput = visr.ParameterInput( "objects", self, pml.ObjectVector.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )

        if interpolationSteps is None:
            interpolationSteps = context.period

        self.objectEncoder = HoaObjectEncoder( context, 'HoaEncoder', self,
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

        routings = rbbl.FilterRoutingList()
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
                                                    controlInputs=rcl.FirFilterMatrix.ControlPortConfig.NoInputs,
                                                    fftImplementation = fftImplementation )

        self.audioConnection( self.encoderMatrix.audioPort("out"), self.binauralFilterBank.audioPort("in") )
        self.audioConnection( self.binauralFilterBank.audioPort("out"), self.binauralOutput )

        if headTracking:
            self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            self.coefficientRotator = HoaCoefficientRotation( context, 'coefficientRotator', self,
                                                              numberOfObjects = numberOfObjects,
                                                              hoaOrder = maxHoaOrder)
            self.parameterConnection( self.trackingInput, self.coefficientRotator.parameterPort("tracking") )
            self.parameterConnection( self.objectEncoder.parameterPort("coefficientOutput"),
                                     self.coefficientRotator.parameterPort("coefficientInput") )
            self.parameterConnection( self.coefficientRotator.parameterPort("coefficientOutput"),
                                     self.encoderMatrix.parameterPort("gainInput") )
        else:
            self.parameterConnection( self.objectEncoder.parameterPort("coefficientOutput"),
                                     self.encoderMatrix.parameterPort("gainInput") )
