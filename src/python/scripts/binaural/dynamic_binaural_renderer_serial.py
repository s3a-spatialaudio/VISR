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

class DynamicBinauralRendererSerial(visr.CompositeComponent ):    
        def __init__( self,
                     context, name, parent, 
                     numberOfObjects,
                     port,
                     baud,
                     enableSerial = True,
                     dynamicITD = True,
                     dynamicILD = True,
                     hrirInterpolation = True
                     ):
            super( DynamicBinauralRendererSerial, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectDataInput", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )

            self.dynamicBinauralRenderer = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", self, numberOfObjects, 
                                                                     headTracking = enableSerial,
                                                                     dynITD = dynamicITD,
                                                                     dynILD = dynamicILD,
                                                                     hrirInterp = hrirInterpolation
                                                                   )
            if enableSerial:
                self.serialReader = serialReader(context, "Controller", self,port, baud )
                self.parameterConnection( self.serialReader.parameterPort("orientation"), self.dynamicBinauralRenderer.parameterPort("tracking"))

            self.parameterConnection( self.objectVectorInput, self.dynamicBinauralRenderer.parameterPort("objectVector"))
            self.audioConnection(  self.objectSignalInput, self.dynamicBinauralRenderer.audioPort("audioIn"))
            self.audioConnection( self.dynamicBinauralRenderer.audioPort("audioOut"), self.binauralOutput)

