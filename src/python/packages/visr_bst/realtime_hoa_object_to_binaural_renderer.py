#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct 18 14:04:28 2017

@author: gc1y17
"""
from hoa_binaural_renderer import HoaBinauralRenderer
from serial_reader import serialReader
import visr
import pml

class HoaBinauralRendererSerial(visr.CompositeComponent ):    
        def __init__( self,
                     context, name, parent, 
                     numberOfObjects,
                     port,
                     baud, 
                     maxHoaOrder,
                     sofaFile,
                     interpolationSteps = None,
                     headTracking = True,
                     objectChannelAllocation = False,
                     headTrackingCalibrationPort = None
                     ):
            super( HoaBinauralRendererSerial, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )

            self.hoaBinauralRenderer = HoaBinauralRenderer( context, "HoaBinauralRenderer", self,
                                                            numberOfObjects = numberOfObjects,
                                                            maxHoaOrder = maxHoaOrder,
                                                            sofaFile = sofaFile,
                                                            interpolationSteps = interpolationSteps,
                                                            headTracking = headTracking
                                                            )
            if headTracking:
                ##                WITH AUDIOLAB ORIENTATION OFFSET
#                self.serialReader = serialReader(context, "Controller", self,port, baud, yawOffset=220,rollOffset=-180, yawRightHand=True )
                
##                WITH MY OFFICE DESK ORIENTATION OFFSET


                calibrationInputPresent = not headTrackingCalibrationPort is None
                self.serialReader = serialReader(context, "RazorHeadtrackerReceiver", self, port, baud, yawOffset=90,rollOffset=-180, yawRightHand=True,
                                                 calibrationInput = calibrationInputPresent)

                self.parameterConnection( self.serialReader.parameterPort("orientation"), self.hoaBinauralRenderer.parameterPort("headTracking"))

                if calibrationInputPresent:
                    self.calibrationInput = visr.ParameterInput( "headTrackingCalibration",
                                                                self, pml.StringParameter.staticType,
                                                                pml.MessageQueueProtocol.staticType,
                                                                pml.EmptyParameterConfig() )
                    self.parameterConnection( self.calibrationInput,
                                             self.serialReader.parameterPort("calibration"))



            self.parameterConnection( self.objectVectorInput, self.hoaBinauralRenderer.parameterPort("objectVector"))
            self.audioConnection(  self.objectSignalInput, self.hoaBinauralRenderer.audioPort("audioIn"))
            self.audioConnection( self.hoaBinauralRenderer.audioPort("audioOut"), self.binauralOutput)

