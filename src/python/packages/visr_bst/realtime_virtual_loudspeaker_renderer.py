# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import rcl

from virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer

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
                     headTrackingCalibrationPort = None
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
                                      irTruncationLength = irTruncationLength
                                      )
            if enableSerial:
                calibrationInputPresent = not headTrackingCalibrationPort is None
                self.serialReader = serialReader(context, "RazorHeadtrackerReceiver", self, port, baud, yawOffset=90,rollOffset=-180, yawRightHand=True,
                                                 calibrationInput = calibrationInputPresent)
                self.parameterConnection( self.serialReader.parameterPort("orientation"), self.virtualLoudspeakerRenderer.parameterPort("tracking"))
                if calibrationInputPresent:
                    self.calibrationTriggerReceiver = rcl.UdpReceiver( context, "CalibrationTriggerReceiver", self, port = headTrackingCalibrationPort )
                    self.parameterConnection( self.calibrationTriggerReceiver.parameterPort("messageOutput"),
                                                                                      self.serialReader.parameterPort("calibration"))

            self.audioConnection(  self.objectSignalInput, self.virtualLoudspeakerRenderer.audioPort("audioIn"))
            self.audioConnection( self.virtualLoudspeakerRenderer.audioPort("audioOut"), self.binauralOutput)

