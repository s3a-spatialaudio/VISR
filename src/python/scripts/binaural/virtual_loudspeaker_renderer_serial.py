#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 31 16:37:58 2017

@author: gc1y17
"""

from virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer
from serial_reader import serialReader
import visr
import rcl

class VirtualLoudspeakerRendererSerial(visr.CompositeComponent ):    
        def __init__( self,
                     context, name, parent, 
                     numLoudspeakers,
                     port,
                     baud, 
                     sofaFile,
                     enableSerial = True,
                     dynITD = True,
                     hrirInterp = True,
                     irTruncationLength = None,
                     headTrackingCalibrationPort = None
                     ):
            super( VirtualLoudspeakerRendererSerial, self ).__init__( context, name, parent )
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
##                WITH AUDIOLAB ORIENTATION OFFSET
#                self.serialReader = serialReader(context, "Controller", self,port, baud, yawOffset=220,rollOffset=-180, yawRightHand=True )
                
##                WITH MY OFFICE DESK ORIENTATION OFFSET
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

