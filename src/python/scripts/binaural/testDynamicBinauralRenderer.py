#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 14 14:55:25 2017

@author: gc1y17
"""

# -*- coding: utf-8 -*-
"""
Created on Fri Sep  8 08:16:32 2017

@author: af5u13
"""

from readSofa import readSofaFile, sph2cart

from dynamic_binaural_controller import DynamicBinauralController
from dynamic_binaural_renderer import DynamicBinauralRenderer

import visr
import pml
import objectmodel as om
import rrl

import numpy as np
import matplotlib.pyplot as plt
import os
from urllib.request import urlretrieve

class DynamicBinauralRendererSerial(visr.CompositeComponent ):    
        def __init__( self,
                     context, name, parent, 
                     numberOfObjects
                     ):
            super( DynamicBinauralRendererSerial, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectDataInput", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
            #to be completed
            

#to be completed
fs = 48000
bufferSize = 256

numBinauralObjects = 12

context = visr.SignalFlowContext( period=bufferSize, samplingFrequency=fs)


controller = DynamicBinauralRenderer( context, "Controller", None,
                  numBinauralObjects
                  )
#to be completed