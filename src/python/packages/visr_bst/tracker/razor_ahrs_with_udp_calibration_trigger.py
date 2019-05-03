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
from rcl import UdpReceiver

from .razor_ahrs import RazorAHRS

class RazorAHRSWithUdpCalibrationTrigger( visr.CompositeComponent ):
    """
    Receiver for the Razor AHRS head tracker with an additional UDP receiver port for calibration, i.e.,
    to define the current look direction as the 'zero', frontal orientation.
    """
    def __init__( self,
                  context, name, parent,
                  *,
                  calibrationPort,
                  ** razorArgs
                  ):
        """
        Constructor.

        context : visr.SignalFlowContext
            Standard visr.Component construction argument, a structure holding the block size and the sampling frequency
        name : string
            Name of the component, Standard visr.Component construction argument
        parent : visr.CompositeComponent
            Containing component if there is one, None if this is a top-level component of the signal flow.
        calibrationPort: int
            A UDP port number. Packets sent to this port trigger the calibration.
        razorArg: keyword list
            Set of parameters to the RazorAHRS. See this class for parameter documentation.
        """
        super( RazorAHRSWithUdpCalibrationTrigger, self ).__init__( context, name, parent )
        self.trackingOutput = visr.ParameterOutput( "orientation", self,
                                              pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )

        razorArgs['calibrationInput'] = True # Reset the keyword argument (if already present)
        self.tracker = RazorAHRS( context, "Tracker", self, **razorArgs )
        self.triggerReceiver = UdpReceiver( context, "CalibrationTriggerReceiver", self,
                                           port = calibrationPort )
        self.parameterConnection( self.triggerReceiver.parameterPort("messageOutput"),
                                 self.tracker.parameterPort("calibration"))
        self.parameterConnection( self.tracker.parameterPort("orientation"),
                                 self.trackingOutput )
