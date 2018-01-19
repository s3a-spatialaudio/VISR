#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 03 10:30:25 2017

@author: gc1y1
"""
from dynamic_binaural_renderer import DynamicBinauralRenderer
from serial_reader import serialReader

import visr
import pml
import rcl

class DynamicBinauralRendererSerial(visr.CompositeComponent ):
        def __init__( self,
                     context, name, parent,
                     numberOfObjects,
                     port,
                     baud,
                     sofaFile,
                     enableSerial = True,
                     dynITD = True,
                     dynILD = True,
                     hrirInterp = True,
                     headTrackingCalibrationPort = None,
                     filterCrossfading = False, 
                     ):
            super( DynamicBinauralRendererSerial, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )

            self.dynamicBinauralRenderer = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", self, numberOfObjects, sofaFile,
                                                                     headTracking = enableSerial,
                                                                     dynITD = dynITD,
                                                                     dynILD = dynILD,
                                                                     hrirInterp = hrirInterp,
                                                                     filterCrossfading=filterCrossfading
                                                                   )
            if enableSerial:
##                WITH AUDIOLAB ORIENTATION OFFSET
#                self.serialReader = serialReader(context, "Controller", self,port, baud, yawOffset=220,rollOffset=-180, yawRightHand=True )

##                WITH MY OFFICE DESK ORIENTATION OFFSET
                calibrationInputPresent = not headTrackingCalibrationPort is None
                self.serialReader = serialReader(context, "RazorHeadtrackerReceiver", self, port, baud, yawOffset=90,rollOffset=-180, yawRightHand=True,
                                                 calibrationInput = calibrationInputPresent)
                self.parameterConnection( self.serialReader.parameterPort("orientation"), self.dynamicBinauralRenderer.parameterPort("tracking"))

                if calibrationInputPresent:
                    self.calibrationInput = visr.ParameterInput( "headTrackingCalibration",
                                                                self, pml.StringParameter.staticType,
                                                                pml.MessageQueueProtocol.staticType,
                                                                pml.EmptyParameterConfig() )
                    self.parameterConnection( self.calibrationInput,
                                             self.serialReader.parameterPort("calibration"))

            self.parameterConnection( self.objectVectorInput, self.dynamicBinauralRenderer.parameterPort("objectVector"))
            self.audioConnection(  self.objectSignalInput, self.dynamicBinauralRenderer.audioPort("audioIn"))
            self.audioConnection( self.dynamicBinauralRenderer.audioPort("audioOut"), self.binauralOutput)

