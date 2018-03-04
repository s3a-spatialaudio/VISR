# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import rcl

from .virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer

# To be removed as soon as the tracking device is passed as a constructor parameter
from .tracker.razor_ahrs import RazorAHRS

class RealtimeVirtualLoudspeakerRenderer(visr.CompositeComponent ):
        def __init__( self,
                     context, name, parent,
                     numLoudspeakers,
                     port,
                     sofaFile,
                     enableSerial = True,
                     dynITD = True,
                     hrirInterp = True,
                     irTruncationLength = None,
                     headTrackingCalibrationPort = None,
                     filterCrossfading = False,
                     interpolatingConvolver = False,
                     fftImplementation = 'default'
                     ):
            super( RealtimeVirtualLoudspeakerRenderer, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numLoudspeakers )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

            self.virtualLoudspeakerRenderer =  VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", self,
                                      numLoudspeakers,
                                      sofaFile,
                                      headTracking = enableSerial,
                                      dynITD = dynITD,
                                      hrirInterp = hrirInterp,
                                      irTruncationLength = irTruncationLength,
                                      filterCrossfading = filterCrossfading,
                                      interpolatingConvolver = interpolatingConvolver,
                                      fftImplementation = fftImplementation
                                      )
            if enableSerial:
                calibrationInputPresent = not headTrackingCalibrationPort is None
                self.serialReader = RazorAHRS(context, "RazorHeadtrackerReceiver", self, port, yawOffset=90,rollOffset=-180, yawRightHand=True,
                                                 calibrationInput = calibrationInputPresent)
                self.parameterConnection( self.serialReader.parameterPort("orientation"), self.virtualLoudspeakerRenderer.parameterPort("tracking"))
                if calibrationInputPresent:
                    self.calibrationTriggerReceiver = rcl.UdpReceiver( context, "CalibrationTriggerReceiver", self, port = headTrackingCalibrationPort )
                    self.parameterConnection( self.calibrationTriggerReceiver.parameterPort("messageOutput"),
                                             self.serialReader.parameterPort("calibration"))

            self.audioConnection(  self.objectSignalInput, self.virtualLoudspeakerRenderer.audioPort("audioIn"))
            self.audioConnection( self.virtualLoudspeakerRenderer.audioPort("audioOut"), self.binauralOutput)
