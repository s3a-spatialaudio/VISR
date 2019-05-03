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

from visr_bst import HoaObjectToBinauralRenderer

class RealtimeHoaObjectToBinauralRenderer(visr.CompositeComponent ):
    """
    VISR component for realtime audio rendering of object-based scenes using a
    Higher-order Ambisonics encoding of point source/plane wave objects and binaural
    rendering of the soiundfield representation.

    It contains a HoaObjectToRenderer component, but optionally adds a receiver
    component for head tracking devices and real-time receipt of object metadata
    from UDP network packets.
    """
    def __init__( self,
                 context, name, parent,
                 *,                       # Only keyword arguments after this point
                 numberOfObjects,
                 maxHoaOrder,
                 sofaFile = None,
                 decodingFilters = None,
                 interpolationSteps = None,
                 headTracking = True,
                 headOrientation = None,
                 objectChannelAllocation = False,
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
        numberOfObjects : int
            The number of audio objects to be rendered.
        maxHoaOrder: int
            HOA order used for encoding the point source and plane wave objects.
        sofaFile: string, optional
            A SOFA file containing the HOA decoding filters. These are expects as a
            2 x (maxHoaIrder+1)^2 array in the field Data.IR
        decodingFilters : numpy.ndarray, optional
            Alternative way to provide the HOA decoding filters. Expects a
            2 x (maxHoaIrder+1)^2 matrix containing FIR coefficients.
        interpolationSteps: int, optional
           Number of samples to transition to new object positions after an update.
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
        super( RealtimeHoaObjectToBinauralRenderer, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

        enableTracking = (headTrackingReceiver is not None)

        self.hoaBinauralRenderer = HoaObjectToBinauralRenderer( context, "HoaBinauralRenderer", self,
                                                        numberOfObjects = numberOfObjects,
                                                        maxHoaOrder = maxHoaOrder,
                                                        sofaFile = sofaFile,
                                                        decodingFilters = decodingFilters,
                                                        interpolationSteps = interpolationSteps,
                                                        headTracking = headTracking,
                                                        headOrientation = headOrientation,
                                                        fftImplementation = fftImplementation
                                                        )

        if sceneReceiveUdpPort is None:
            self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
            self.parameterConnection( self.objectVectorInput,
                                     self.hoaBinauralRenderer.parameterPort("objects"))
        else:
            self.sceneReceiver = rcl.UdpReceiver( context, "SceneReceiver", self,
                                                 port = int(sceneReceiveUdpPort) )
            self.sceneDecoder = rcl.SceneDecoder( context, "SceneDecoder", self )
            self.parameterConnection( self.sceneReceiver.parameterPort("messageOutput"),
                                 self.sceneDecoder.parameterPort("datagramInput") )
            self.parameterConnection( self.sceneDecoder.parameterPort( "objectVectorOutput"),
                                 self.hoaBinauralRenderer.parameterPort("objects"))

        if enableTracking:
            if headTrackingPositionalArguments == None:
                headTrackingPositionalArguments = ()
            if headTrackingKeywordArguments == None:
                headTrackingKeywordArguments = {}
            self.trackingDevice = headTrackingReceiver(context, "HeadTrackingReceiver", self,
                                                *headTrackingPositionalArguments,
                                                **headTrackingKeywordArguments )
            self.parameterConnection( self.trackingDevice.parameterPort("orientation"),
                                     self.hoaBinauralRenderer.parameterPort("tracking"))

        self.audioConnection(  self.objectSignalInput, self.hoaBinauralRenderer.audioPort("audioIn"))
        self.audioConnection( self.hoaBinauralRenderer.audioPort("audioOut"), self.binauralOutput)

