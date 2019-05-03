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

import visr
import pml
import rbbl
import rcl

from visr_bst.util import readSofaFile, deg2rad

from visr_bst import VirtualLoudspeakerController

import numpy as np

class VirtualLoudspeakerRenderer( visr.CompositeComponent ):
    """
    Signal flow for rendering binaural output for a multichannel signal reproduced over a virtual loudspeaker array with corresponding BRIR data.
    """
    def __init__( self,
                 context, name, parent,
                 *,                     # This ensures that the remaining arguments are given as keyword arguments.
                 sofaFile = None,
                 hrirPositions = None,
                 hrirData = None,
                 hrirDelays = None,
                 headOrientation = None,
                 headTracking = True,
                 dynamicITD = False,
                 hrirInterpolation = False,
                 irTruncationLength = None,
                 filterCrossfading = False,
                 interpolatingConvolver = False,
                 staticLateSofaFile = None,
                 staticLateFilters = None,
                 staticLateDelays = None,
                 fftImplementation = 'default'
                 ):
        """
        Constructor.

        Parameters
        ----------
        context : visr.SignalFlowContext
            Standard visr.Component construction argument, a structure holding the block size and the sampling frequency
        name : string
            Name of the component, Standard visr.Component construction argument
        parent : visr.CompositeComponent
            Containing component if there is one, None if this is a top-level component of the signal flow.
        sofaFile: string
            BRIR database provided as a SOFA file. This is an alternative to the hrirPosition, hrirData
            (and optionally hrirDelays) argument. Default None means that hrirData and hrirPosition must be provided.
        hrirPositions : numpy.ndarray
            Optional way to provide the measurement grid for the BRIR listener view directions. If a
            SOFA file is provided, this is optional and overrides the listener view data in the file.
            Otherwise this argument is mandatory. Dimension #grid directions x (dimension of position argument)
        hrirData: numpy.ndarray
            Optional way to provide the BRIR data. Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
        hrirDelays: numpy.ndarray
            Optional BRIR delays. If a SOFA file is given, this  argument overrides a potential delay setting from the file. Otherwise, no extra delays
            are applied unless this option is provided. Dimension: #grid directions  x #ears(2) x # loudspeakers
        headOrientation : array-like
            Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
            or the initial view direction
        headTracking: bool
            Whether dynamic headTracking is active. If True, an control input "tracking" is created.
        dynamicITD: bool
            Whether the delay part of th BRIRs is applied separately to the (delay-free) BRIRs.
        hrirInterpolation: bool
            Whether BRIRs are interpolated for the current head oriention. If False, a nearest-neighbour interpolation is used.
        irTruncationLength: int
            Maximum number of samples of the BRIR impulse responses. Functional only if the BRIR is provided in a SOFA file.
        filterCrossfading: bool
            Whether dynamic BRIR changes are crossfaded (True) or switched immediately (False)
        interpolatingConvolver: bool
            Whether the interpolating convolver option is used. If True, the convolver stores all BRIR filters, and the controller sends only
            interpolation coefficient messages to select the BRIR filters and their interpolation ratios.
        staticLateSofaFile: string, optional
            Name of a file containing a static (i.e., head orientation-independent) late part of the BRIRs.
            Optional argument, might be used as an alternative to the staticLateFilters argument, but these options are mutually exclusive.
            If neither is given, no static late part is used. The fields 'Data.IR' and the 'Data.Delay' are used.
        staticLateFilters: numpy.ndarray, optional
            Matrix containing a static, head position-independent part of the BRIRs. This option is mutually exclusive to
            staticLateSofaFile. If none of these is given, no separate static late part  is rendered.
            Dimension: 2 x #numberOfLoudspeakers x firLength
        staticLateDelays: numpy.ndarray, optional
            Time delay of the late static BRIRs per loudspeaker. Optional attribute,
            only used if late static BRIR coefficients are provided.
            Dimension: 2 x #loudspeakers
        fftImplementation: string
            The FFT implementation to be used in the convolver. the default value selects the system default.
        """
        if (hrirData is not None) and (sofaFile is not None):
            raise ValueError( "Exactly one of the arguments sofaFile and hrirData must be present." )
        if sofaFile is not None:
            [ sofaHrirPositions, hrirData, sofaHrirDelays ] = readSofaFile( sofaFile,
                                                         truncationLength=irTruncationLength,
                                                         truncationWindowLength=16 )
            # If hrirDelays is not provided as an argument, use the one retrieved from the SOFA file
            if hrirDelays is None:
                hrirDelays = sofaHrirDelays
            # Use the positions obtained from the SOFA file only if the argument is not set
            if hrirPositions is None:
                hrirPositions = sofaHrirPositions

        # Crude check for 'horizontal-only' listener view directions
        if np.max( np.abs(hrirPositions[:,1])) < deg2rad( 1 ):
            hrirPositions = hrirPositions[ :, [0,2] ] # transform to polar coordinates

        numberOfLoudspeakers = hrirData.shape[2]

        super(VirtualLoudspeakerRenderer,self).__init__( context, name, parent )
        self.loudspeakerSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfLoudspeakers )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
        if headTracking:
            self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                          pml.DoubleBufferingProtocol.staticType,
                                          pml.EmptyParameterConfig() )

        # Check consistency between HRIR positions and HRIR data
        if (hrirPositions.shape[0] != hrirData.shape[0]):
            raise ValueError( "The number of HRIR positions is inconsistent with the dimension of the HRIR data." )

        # Additional safety check (is tested in the controller anyway)
        if dynamicITD:
            if (hrirDelays is None) or (hrirDelays.ndim != hrirData.ndim-1) or (hrirDelays.shape != hrirData.shape[0:-1] ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must match the first dimensions of the hrir data matrix.' )

        self.virtualLoudspeakerController = VirtualLoudspeakerController( context, "VirtualLoudspeakerController", self,
                                                                  hrirPositions=hrirPositions,
                                                                  hrirData = hrirData,
                                                                  headTracking = headTracking,
                                                                  dynamicITD = dynamicITD,
                                                                  hrirInterpolation = hrirInterpolation,
                                                                  hrirDelays = hrirDelays,
                                                                  interpolatingConvolver=interpolatingConvolver
                                                                  )

        if headTracking:
            self.parameterConnection( self.trackingInput, self.virtualLoudspeakerController.parameterPort("headTracking"))


        # Define the routing for the binaural convolver such that it matches the organisation of the
        # flat BRIR matrix.
        filterRouting = rbbl.FilterRoutingList()

        firLength = hrirData.shape[-1]

        if dynamicITD:
            self.delayVector = rcl.DelayVector( context, "delayVector", self,
                                               numberOfLoudspeakers*2, interpolationType="lagrangeOrder3", initialDelay=0,
                                               controlInputs=rcl.DelayVector.ControlPortConfig.Delay,
                                               methodDelayPolicy=rcl.DelayMatrix.MethodDelayPolicy.Add,
                                               initialGain=1.0,
                                               interpolationSteps=context.period)

            self.audioConnection(self.loudspeakerSignalInput, [ i % numberOfLoudspeakers for i in range(numberOfLoudspeakers*2)],
                                 self.delayVector.audioPort("in"), range(0,2*numberOfLoudspeakers ) )

            for idx in range(0, numberOfLoudspeakers ):
                filterRouting.addRouting( idx, 0, idx, 1.0 )
                filterRouting.addRouting( idx+numberOfLoudspeakers, 1, idx+numberOfLoudspeakers, 1.0 )

            if interpolatingConvolver:
                if filterCrossfading:
                    interpolationSteps = context.period
                else:
                    interpolationSteps = 0

                numFilters = np.prod( hrirData.shape[:-1])
                filterReshaped = np.reshape( hrirData, (numFilters,firLength))
                self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=2*numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=numFilters,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 numberOfInterpolants=2, # TODO: Find out from
                                                 transitionSamples=interpolationSteps,
                                                 filters = filterReshaped,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.InterpolatingFirFilterMatrix.ControlPortConfig.Filters,
                                                 fftImplementation=fftImplementation
                                                 )

            elif filterCrossfading:
                self.convolver = rcl.CrossfadingFirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=2*numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=2*numberOfLoudspeakers,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 transitionSamples=context.period,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.CrossfadingFirFilterMatrix.ControlPortConfig.Filters,
                                                 fftImplementation=fftImplementation
                                                 )
            else:
                self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                             numberOfInputs=2*numberOfLoudspeakers,
                             numberOfOutputs=2,
                             maxFilters=2*numberOfLoudspeakers,
                             filterLength=firLength,
                             maxRoutings=2*numberOfLoudspeakers,
                             routings=filterRouting,
                             controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                             fftImplementation=fftImplementation
                             )

            self.audioConnection( self.delayVector.audioPort("out"), self.convolver.audioPort("in"), )
            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )

        else: # no dynamic ITD
            for idx in range(0, numberOfLoudspeakers ):
                filterRouting.addRouting( idx, 0, idx, 1.0 )
                filterRouting.addRouting( idx, 1, idx+numberOfLoudspeakers, 1.0 )
            if interpolatingConvolver:
                if filterCrossfading:
                    interpolationSteps = context.period
                else:
                    interpolationSteps = 0

                #filterReshaped = np.concatenate( (hrirData[:,0,...],hrirData[:,1,...]), axis=1 )
                numFilters = np.prod(np.array(hrirData.shape[0:-1]))
                filterReshaped = np.reshape( hrirData, (numFilters, firLength ))
                self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=numFilters,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 numberOfInterpolants=2, # TODO: Find out from
                                                 transitionSamples=interpolationSteps,
                                                 filters = filterReshaped,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.InterpolatingFirFilterMatrix.ControlPortConfig.Interpolants,
                                                 fftImplementation=fftImplementation
                                                 )
            elif filterCrossfading:
                self.convolver = rcl.CrossfadingFirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=2*numberOfLoudspeakers,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 transitionSamples=context.period,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.CrossfadingFirFilterMatrix.ControlPortConfig.Filters,
                                                 fftImplementation=fftImplementation
                                                 )
            else:
                self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=2*numberOfLoudspeakers,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                 fftImplementation=fftImplementation
                                                 )
            self.audioConnection(self.loudspeakerSignalInput,
                                 self.convolver.audioPort("in") )

        if interpolatingConvolver:
            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("interpolatorOutput"),self.convolver.parameterPort("interpolantInput") )
        else:
            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )

        # Optionally use static filters for the late part.
        if (staticLateSofaFile is not None) and (staticLateFilters is not None):
            raise ValueError("The arguments 'staticLateSofaFile' and 'staticLateFilters' cannot both be given." )
        if (staticLateSofaFile is not None):
            latePos, lateFilters, lateDelay = readSofaFile( staticLateSofaFile )
            staticLateDelays = np.squeeze( lateDelay )
            staticLateFilters = np.squeeze(lateFilters)

        if( staticLateFilters is not None ):
            flatDelays = staticLateDelays.flatten( order='C' )
            self.staticLateDelays = rcl.DelayVector( context, 'staticLateDelays', self,
                                                    2*numberOfLoudspeakers,
                                                    interpolationSteps=context.period,
                                                    interpolationType='nearestSample',
                                                    initialGain = np.ones( (numberOfLoudspeakers), dtype=np.float32 ),
                                                    initialDelay = flatDelays )
            lateFilterRouting = rbbl.FilterRoutingList(
                    [rbbl.FilterRouting(i,
                                        i//numberOfLoudspeakers, i, 1.0)
                      for i in range(2*numberOfLoudspeakers) ] )

            flatLateFilters = np.reshape( staticLateFilters,
                                         (2*numberOfLoudspeakers, -1 ),
                                         order='C' )
            self.staticLateFilters = rcl.FirFilterMatrix( context, "staticlateFilters", self,
                                                         numberOfInputs = 2*numberOfLoudspeakers,
                                                         numberOfOutputs = 2,
                                                         filterLength=staticLateFilters.shape[-1],
                                                         maxFilters = 2*numberOfLoudspeakers,
                                                         maxRoutings = 2*numberOfLoudspeakers,
                                                         routings = lateFilterRouting,
                                                         filters = flatLateFilters,
                                                         fftImplementation = fftImplementation
                                                         )
            self.audioConnection(sendPort=self.loudspeakerSignalInput,
                                 sendIndices = list(range(numberOfLoudspeakers))+list(range(numberOfLoudspeakers)),
                                 receivePort=self.staticLateDelays.audioPort("in") )
            self.audioConnection(sendPort=self.staticLateDelays.audioPort("out"),
                                 receivePort=self.staticLateFilters.audioPort("in") )
            self.earlyLateSum = rcl.Add( context, "earlyLateSum", self,
                                        numInputs=2, width=2 )
            self.audioConnection( self.convolver.audioPort("out"), self.earlyLateSum.audioPort("in0") )
            self.audioConnection( self.staticLateFilters.audioPort("out"),
                                 self.earlyLateSum.audioPort("in1") )
            self.audioConnection( self.earlyLateSum.audioPort("out"), self.binauralOutput)
        else:
            self.audioConnection( self.convolver.audioPort("out"), self.binauralOutput)
