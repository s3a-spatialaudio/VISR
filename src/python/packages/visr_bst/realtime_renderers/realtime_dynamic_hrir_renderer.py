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
import rcl

from visr_bst import DynamicHrirRenderer

from visr_bst.util import readSofaFile

class RealtimeDynamicHrirRenderer(visr.CompositeComponent ):
    """
    VISR component for realtime audio rendering of object-based scenes using a 'dynamic HRIR' approach.

    It contains a DynamicHrirRenderer component, but optionally adds a receiver
    component for head tracking devices and real-time receipt of object metadata
    from UDP network packets.
    """
    def __init__( self,
                 context, name, parent,
                 *,
                 numberOfObjects,
                 sofaFile = None,
                 hrirPositions = None,
                 hrirData = None,
                 hrirDelays = None,
                 headOrientation = None,
                 dynamicITD = False,
                 dynamicILD = False,
                 hrirInterpolation = False,
                 filterCrossfading = False,
                 fftImplementation = "default",
                 headTrackingReceiver = None,
                 headTrackingPositionalArguments = None,
                 headTrackingKeywordArguments = None,
                 sceneReceiveUdpPort = None
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
        numberOfObjects: int
            Maximum number of audio objects
        sofaFile: str, optional
            Optional SOFA for loading loaded the HRIR and associated data (HRIR measurement positions and delays)
            If not provided, the information must be provided by the hrirPositions and hrirData arguments.
        hrirPositions: numpy.ndarray, optional
            Optional way to provide the measurement grid for the BRIR listener view directions.
            If a SOFA file is provided, this is optional and overrides the listener view data
            in the file. Otherwise this argument is mandatory.
            Dimension #grid directions x (dimension of position argument)
        hrirData: numpy.ndarray, optional
            Optional way to provide the BRIR data.
            Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
        hrirDelays: numpy.ndarray, optional
            Optional BRIR delays. If a SOFA file is given, this  argument overrides
            a potential delay setting from the file. Otherwise, no extra delays
            are applied unless this option is provided.
            Dimension: #grid directions  x #ears(2) x # loudspeakers
        headOrientation: array-like, optional
            Head orientation in spherical coordinates (2- or 3-element vector or list).
            Either a static orientation (when no tracking is used), or the
            initial view direction
        dynamicITD: bool, optional
            Whether the ITD is applied separately. That requires preprocessed HRIR data
        dynamicILD: bool, optional
            Whether the ILD is computed and applied separately. At the moment this feature is not used (apart from applying the object gains)
        hrirInterpolation: bool, optional
            Whether the controller supports interpolation between neighbouring HRTF grid
            points. False means nearest neighbour (no interpolation), True
            enables barycentric interpolation.
        filterCrossfading: bool, optional
            Use a crossfading FIR filter matrix to avoid switching artifacts.
        fftImplementation: string, optional
            The FFT implementation to use. Default value enables VISR's default
            FFT library for the platform.
        headTrackingReceiver: class type, optional
            Class of the head tracking recveiver, None (default value) disables dynamic head tracking.
        headTrackingPositionalArguments: tuple optional
            Positional arguments passed to the constructor of the head tracking receiver object.
            Must be a tuple. If there is only a single argument, a trailing comma must be added.
        headTrackingKeywordArguments: dict, optional
            Keyword arguments passed to the constructor of the head tracking receiver. Must be a dictionary (dict)
        sceneReceiveUdpPort: int, optional
            A UDP port number where scene object metadata (in the S3A JSON format) is to be received).
            If not given (default), no network receiver is instantiated, and the object exposes a
            top-level parameter input port "objectVectorInput"
        """
        super( RealtimeDynamicHrirRenderer, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

        enableTracking = (headTrackingReceiver is not None)

        # Handle loading of HRIR data from either a SOFA file or the the matrix arguments.
        if (hrirData is not None) == (sofaFile is not None):
            raise ValueError( "Exactly one of the arguments sofaFile and hrirData must be present." )
        if sofaFile is not None:
            [ sofaHrirPositions, hrirData, sofaHrirDelays ] = readSofaFile( sofaFile )
            # If hrirDelays is not provided as an argument, use the one retrieved from the SOFA file
            if hrirDelays is None:
                hrirDelays = sofaHrirDelays
            # Use the positions obtained from the SOFA file only if the argument is not set
            if hrirPositions is None:
                hrirPositions = sofaHrirPositions

        self.dynamicHrirRenderer = DynamicHrirRenderer( context, "DynamicBinauralRenderer", self,
                                                       numberOfObjects = numberOfObjects,
                                                       hrirPositions = hrirPositions,
                                                       hrirData = hrirData,
                                                       hrirDelays = hrirDelays,
                                                       headOrientation = headOrientation,
                                                       headTracking = enableTracking,
                                                       dynamicITD = dynamicITD,
                                                       dynamicILD = dynamicILD,
                                                       hrirInterpolation = hrirInterpolation,
                                                       filterCrossfading = filterCrossfading,
                                                       fftImplementation = fftImplementation
                                                       )

        if sceneReceiveUdpPort is None:
            self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
            self.parameterConnection( self.objectVectorInput,
                                     self.dynamicHrirRenderer.parameterPort("objectVector"))

        else:
            self.sceneReceiver = rcl.UdpReceiver( context, "SceneReceiver", self,
                                                 port = int(sceneReceiveUdpPort) )
            self.sceneDecoder = rcl.SceneDecoder( context, "SceneDecoder", self )
            self.parameterConnection( self.sceneReceiver.parameterPort("messageOutput"),
                                 self.sceneDecoder.parameterPort("datagramInput") )
            self.parameterConnection( self.sceneDecoder.parameterPort( "objectVectorOutput"),
                                 self.dynamicHrirRenderer.parameterPort("objectVector"))
        if enableTracking:
            if headTrackingPositionalArguments == None:
                headTrackingPositionalArguments = ()
            if headTrackingKeywordArguments == None:
                headTrackingKeywordArguments = {}
            self.trackingDevice = headTrackingReceiver(context, "HeadTrackingReceiver", self,
                                                *headTrackingPositionalArguments,
                                                **headTrackingKeywordArguments )
            self.parameterConnection( self.trackingDevice.parameterPort("orientation"), self.dynamicHrirRenderer.parameterPort("tracking"))

        self.audioConnection(  self.objectSignalInput, self.dynamicHrirRenderer.audioPort("audioIn"))
        self.audioConnection( self.dynamicHrirRenderer.audioPort("audioOut"), self.binauralOutput)
