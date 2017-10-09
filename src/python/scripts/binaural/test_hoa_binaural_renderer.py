#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 09 2017

@author: Andreas Franck
"""

from hoa_binaural_renderer import HoaBinauralRenderer
from rotationFunctions import sph2cart
import visr
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt

import os

fs = 48000

numBinauralObjects = 12

maxHoaOrder = 3

blockSize = 512

parameterUpdatePeriod = 1
numBlocks = 1024;
#numBlocks = 64;
signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)
numOutputChannels = 2;


currDir = os.getcwd()
sofaFile = os.path.join( currDir, './data/bbc_hoa2bin_sofa/Gauss_O3_ku100_dualband_energy.sofa' )

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

graph = HoaBinauralRenderer( context, "HoaBinauralRenderer", None,
                            numberOfObjects = numBinauralObjects,
                            maxHoaOrder = maxHoaOrder,
                            sofaFile = sofaFile,
                            interpolationSteps = blockSize,
                            headTracking = False
                            )

result,messages = rrl.checkConnectionIntegrity(graph)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( graph )

paramInput = flow.parameterReceivePort('objectDataInput')

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,100] = 1.0 # Discrete Dirac pulse

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

az = 0.025
el = 0.1 * np.sin( 0.025)   
r = 1
x,y,z = sph2cart( az, el, r )
ps1 = objectmodel.PointSource(0)
ps1.x = x
ps1.y = y
ps1.z = z
ps1.level = 0.5
ps1.groupId = 5
ps1.priority = 5
ps1.resetNumberOfChannels(1)
ps1.setChannelIndex(0,ps1.objectId)

ov = paramInput.data()
ov.clear()
ov.insert( ps1 )
paramInput.swapBuffers()

for blockIdx in range(0,numBlocks):
    
    az = 0.025 * blockIdx
    el = 0.1 * np.sin( 0.025 * blockIdx )
    
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    #print(str(inputBlock.shape))
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure()
plt.plot( t, outputSignal[0,:], 'b-',t, outputSignal[1,:], 'r-')
plt.show()