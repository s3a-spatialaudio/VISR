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

from visr_bst import HoaBinauralRenderer

class RealtimeHoaBinauralRenderer(visr.CompositeComponent ):
    """
    VISR component for realtime audio rendering of Higher-order Ambisonics (HOA) audio.

    It contains a HoaObjectToRenderer component, but optionally adds a receiver
    component for head tracking devices
    """
    def __init__( self,
                 context, name, parent,
                 *,                       # Only keyword arguments after this point
                 hoaOrder = None,
                 sofaFile = None,
                 decodingFilters = None,
                 interpolationSteps = None,
                 headTracking = True,
                 headOrientation = None,
                 fftImplementation = "default",
                 headTrackingReceiver = None,
                 headTrackingPositionalArguments = None,
                 headTrackingKeywordArguments = None
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
        hoaOrder: optional, int or None
            HOA order used for encoding the point source and plane wave objects.
            If not provided, the order is determined from the number of decoding filters (either passed as a matrix or in
            a SOFA file)
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
        """
        super( RealtimeHoaBinauralRenderer, self ).__init__( context, name, parent )
        enableTracking = (headTrackingReceiver is not None)

        self.hoaBinauralRenderer = HoaBinauralRenderer( context, "HoaBinauralRenderer", self,
                                                        hoaOrder = hoaOrder,
                                                        sofaFile = sofaFile,
                                                        decodingFilters = decodingFilters,
                                                        interpolationSteps = interpolationSteps,
                                                        headTracking = headTracking,
                                                        headOrientation = headOrientation,
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
            self.parameterConnection( self.trackingDevice.parameterPort("orientation"),
                                     self.hoaBinauralRenderer.parameterPort("tracking"))

        self.hoaSignalInput = visr.AudioInputFloat( "audioIn", self,
                                                   self.hoaBinauralRenderer.audioPort("audioIn").width )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

        self.audioConnection(  self.hoaSignalInput, self.hoaBinauralRenderer.audioPort("audioIn"))
        self.audioConnection( self.hoaBinauralRenderer.audioPort("audioOut"), self.binauralOutput)
