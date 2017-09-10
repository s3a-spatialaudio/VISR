# -*- coding: utf-8 -*-
"""
Created on Fri Sep  8 08:16:32 2017

@author: af5u13
"""

from readSofa import readSofaFile, sph2cart

from dynamic_binaural_controller import DynamicBinauralController

import visr
import pml
import objectmodel as om
import rrl

import numpy as np
import matplotlib.pyplot as plt

fs = 48000
bufferSize = 256

numBinauralObjects = 12

context = visr.SignalFlowContext( period=bufferSize, samplingFrequency=fs)

hrirFile = '/home/andi/dev/binaural/dtf b_nh169.sofa'

[ hrirPos, hrirData ] = readSofaFile( hrirFile )

controller = DynamicBinauralController( context, "Controller", None,
                  numBinauralObjects,
                  hrirPos, hrirData,
                  useHeadTracking = False,
                  dynamicITD = True,
                  dynamicILD = True,
                  hrirInterpolation = False
                  )

flow = rrl.AudioSignalFlow( controller )

objectInput = flow.parameterReceivePort( "objectVector" )

filterOutput = flow.parameterSendPort("filterOutput")

numPos = 72
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

for blockIdx in range(0,numPos):
    az = azSequence[blockIdx]
    x = np.cos( az )
    y = np.sin( az )
    z = 0
    ps1 = om.PointSource( 0 )
    ps1.position = [x,y,z]
    ps1.level = 0.25
    ps1.channels = [0]
    
    ps2 = om.PointSource( 1 )
    ps2.position = [x,-y,z]
    ps2.level = 0.25
    ps2.channels = [1]
    
    objectInput.data().assign( om.ObjectVector([ps1, ps2]) )
    objectInput.swapBuffers()
    
    flow.process()
    
    while not filterOutput.empty():
        filterSet = filterOutput.front()
        filterOutput.pop()
        del filterSet

