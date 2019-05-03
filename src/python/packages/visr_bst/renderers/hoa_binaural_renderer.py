# -*- coding: utf-8 -*-

# Copyright (C) 2017-2018 Andreas Franck and Giacomo Costantini
# Copyright (C) 2017-2018 University of Southampton

# VISR Binaural Synthesis Toolkit (BST)
# Authors: Andreas Franck and Giacomo Costantini
# Project page: http://cvssp.org/data/s3a/public/BinauralSynthesisToolkit/


# The Binaural Synthesis Toolkit is provided under the ISC (Internet Systems Consortium) license
# https://www.isc.org/downloads/software-support-policy/isc-license/ :

# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


# We kindly ask to acknowledge the use of this software in publications or software.
# Paper citation:
# Andreas Franck, Giacomo Costantini, Chris Pike, and Filippo Maria Fazi,
# “An Open Realtime Binaural Synthesis Toolkit for Audio Research,” in Proc. Audio Eng.
# Soc. 144th Conv., Milano, Italy, 2018, Engineering Brief.
# http://www.aes.org/e-lib/browse.cfm?elib=19525

# The Binaural Synthesis Toolkit is based on the VISR framework. Information about the VISR,
# including download, setup and usage instructions, can be found on the VISR project page
# http://cvssp.org/data/s3a/public/VISR .

import numpy as np

# Core VISR packages
import visr
import rbbl
import pml
import rcl

from visr_bst.hoa_components import HoaRotationMatrixCalculator
from visr_bst.util import readSofaFile

class HoaBinauralRenderer( visr.CompositeComponent ):
    """
    Component to render binaural audio from plane wave and point source objects using an Higher Order Ambisonics (HOA)
    algorithm.
    """
    def __init__( self,
                 context, name, parent,
                 hoaOrder = None,
                 sofaFile = None,
                 decodingFilters = None,
                 interpolationSteps = None,
                 headOrientation = None,
                 headTracking = True,
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
        hoaOrder: int or None
            The maximum HOA order that can be reproduced. If None, the HOA order is deduced
            from the first dimension of the HOA filters (possibly contained in a SOFA file).
        sofaFile: string or NoneType
            A file in SOFA format containing the decoding filters. This expects the filters in the
            field 'Data.IR', dimensions (hoaOrder+1)**2 x 2 x irLength. If None, then the filters
            must be provided in 'decodingFilters' parameter.
        decodingFilters : numpy.ndarray or NoneType
            Alternative way to provide the HOA decoding filters.
        interpolationSteps: int, optional
           Number of samples to transition to new object positions after an update.
        headOrientation : array-like
            Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
            or the initial view direction
        headTracking: bool
            Whether dynamic head tracking is active.
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

        if hoaOrder is None:
            numHoaCoeffs = filters.shape[0]
            orderP1 = int(np.floor(np.sqrt(numHoaCoeffs)))
            if orderP1**2 != numHoaCoeffs:
                raise ValueError( "If hoaOrder is not given, the number of HOA filters must be a square number" )
            hoaOrder = orderP1 - 1
        else:
            numHoaCoeffs = (hoaOrder+1)**2

        if filters.ndim != 3 or filters.shape[1] != 2 or filters.shape[0] < numHoaCoeffs:
            raise ValueError( "HoaObjectToBinauralRenderer: the filter data must be a 3D matrix where the second dimension is 2 and the first dimension is equal or larger than (hoaOrder+1)^2." )

        # Set default value for fading between interpolation
        if interpolationSteps is None:
            interpolationSteps = context.period

        super( HoaBinauralRenderer, self ).__init__( context, name, parent )
        self.hoaSignalInput = visr.AudioInputFloat( "audioIn", self, numHoaCoeffs )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

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

        if headTracking or (headOrientation is not None):

            numMatrixCoeffs = ((hoaOrder+1)*(2*hoaOrder+1)*(2*hoaOrder+3))//3


            self.rotationCalculator = HoaRotationMatrixCalculator( context, "RotationCalculator", self,
                                                                  hoaOrder,
                                                                  dynamicOrientation = headTracking,
                                                                  initialOrientation = headOrientation
                                                                  )

            rotationMatrixRoutings = rbbl.SparseGainRoutingList()
            for oIdx in range(hoaOrder+1):
                entryStart = (oIdx*(2*oIdx-1)*(2*oIdx+1)) // 3
                diagStart = oIdx**2
                for rowIdx in range( 2*oIdx+1 ):
                    row = diagStart + rowIdx
                    colsPerRow = 2*oIdx+1
                    for colIdx in range( 2*oIdx+1 ):
                        col = diagStart + colIdx
                        entryIdx = entryStart + rowIdx * colsPerRow + colIdx
                        rotationMatrixRoutings.addRouting( entryIdx, row, col, 0.0 )

            self.rotationMatrix = rcl.SparseGainMatrix( context, "rotationMatrix", self,
                                             numberOfInputs = numHoaCoeffs,
                                             numberOfOutputs = numHoaCoeffs,
                                             interpolationSteps = interpolationSteps,
                                             maxRoutingPoints = numMatrixCoeffs,
                                             initialRoutings = rotationMatrixRoutings,
                                             controlInputs = rcl.SparseGainMatrix.ControlPortConfig.Gain )
            self.audioConnection( self.hoaSignalInput, self.rotationMatrix.audioPort("in") )
            self.audioConnection( self.rotationMatrix.audioPort("out"), self.binauralFilterBank.audioPort("in") )
            self.parameterConnection( self.rotationCalculator.parameterPort("coefficients"),
                                    self.rotationMatrix.parameterPort( "gainInput" ) )

            if headTracking:
                self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
                self.parameterConnection( self.trackingInput,
                                         self.rotationCalculator.parameterPort("orientation") )
        else:
            self.audioConnection( self.hoaSignalInput, self.binauralFilterbank.audioPort("in") )

        self.audioConnection( self.binauralFilterBank.audioPort("out"), self.binauralOutput )
