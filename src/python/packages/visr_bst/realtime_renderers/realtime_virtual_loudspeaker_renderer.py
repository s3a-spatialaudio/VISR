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

import visr

from visr_bst import VirtualLoudspeakerRenderer

from visr_bst.util import readSofaFile, deg2rad

class RealtimeVirtualLoudspeakerRenderer(visr.CompositeComponent ):
    """
    Binaural renderer to transform a set of loudspeaker signals into a binaural output.

    This class extends visr_bst.VirtualLoudspeakerRenderer by a configurable head tracking receiver,
    making it suitable for realtime use.
    """
    def __init__( self,
                 context, name, parent,
                 *,                                  # No positional arguments past this point.
                 sofaFile = None,
                 hrirPositions = None,
                 hrirData = None,
                 hrirDelays = None,
                 headOrientation = None,
                 dynamicITD = True,
                 hrirInterpolation = True,
                 irTruncationLength = None,
                 filterCrossfading = False,
                 interpolatingConvolver = False,
                 staticLateSofaFile = None,
                 staticLateFilters = None,
                 staticLateDelays = None,
                 headTrackingReceiver = None,
                 headTrackingPositionalArguments = None,
                 headTrackingKeywordArguments = None,
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

        super( RealtimeVirtualLoudspeakerRenderer, self ).__init__( context, name, parent )

        # Handle loading of HRIR data from either a SOFA file or the the matrix arguments.
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

        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfLoudspeakers )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

        enableTracking = (headTrackingReceiver is not None)

        self.virtualLoudspeakerRenderer =  VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", self,
                                                       hrirPositions = hrirPositions,
                                                       hrirData = hrirData,
                                                       hrirDelays = hrirDelays,
                                                       headOrientation = None,
                                                       headTracking = enableTracking,
                                                       dynamicITD = dynamicITD,
                                                       hrirInterpolation = hrirInterpolation,
                                                       irTruncationLength = irTruncationLength,
                                                       filterCrossfading = filterCrossfading,
                                                       interpolatingConvolver = interpolatingConvolver,
                                                       staticLateSofaFile = staticLateSofaFile,
                                                       staticLateFilters = staticLateFilters,
                                                       staticLateDelays = staticLateDelays,
                                                       fftImplementation = fftImplementation
                                                       )
        if enableTracking:
            if headTrackingPositionalArguments == None:
                headTrackingPositionalArguments = ()
            if headTrackingKeywordArguments == None:
                headTrackingKeywordArguments = {}
            self.trackingDevice = headTrackingReceiver(context, "HeadTrackingReceiver", self,
                                                *headTrackingPositionalArguments,
                                                **headTrackingKeywordArguments )
            self.parameterConnection( self.trackingDevice.parameterPort("orientation"), self.virtualLoudspeakerRenderer.parameterPort("tracking"))

        self.audioConnection(  self.objectSignalInput, self.virtualLoudspeakerRenderer.audioPort("audioIn"))
        self.audioConnection( self.virtualLoudspeakerRenderer.audioPort("audioOut"), self.binauralOutput)
