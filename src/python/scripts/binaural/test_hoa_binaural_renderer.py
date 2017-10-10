#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 09 2017

@author: Andreas Franck
"""

from hoa_binaural_renderer import HoaBinauralRenderer
import visr
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt

import os

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

# switch dynamic tracking on and off.
useTracking = True


fs = 48000

numBinauralObjects = 12

# datasets are provided for odd orders 1,3,5,7,9
maxHoaOrder = 9

blockSize = 512

parameterUpdatePeriod = 1
numBlocks = 128;
#numBlocks = 64;
signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)
numOutputChannels = 2;
#numOutputChannels = (maxHoaOrder+1)**2

currDir = os.getcwd()
sofaFile = os.path.join( currDir, './data/bbc_hoa2bin_sofa/Gauss_O%d_ku100_dualband_energy.sofa' % maxHoaOrder )

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

graph = HoaBinauralRenderer( context, "HoaBinauralRenderer", None,
                            numberOfObjects = numBinauralObjects,
                            maxHoaOrder = maxHoaOrder,
                            sofaFile = sofaFile,
                            interpolationSteps = blockSize,
                            headTracking = useTracking
                            )

result,messages = rrl.checkConnectionIntegrity(graph)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( graph )

paramInput = flow.parameterReceivePort('objectVector')


if useTracking:
    trackingInput = flow.parameterReceivePort( 'headTracking' )

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

for blockIdx in range(0,numBlocks):

    az = 0.1 * blockIdx
    el = 0
    x,y,z = sph2cart( az, el, 1.0 )
    ps1 = objectmodel.PointSource(0)
    ps1.x = x
    ps1.y = y
    ps1.z = z
    ps1.level = 0.5
    ps1.groupId = 5
    ps1.priority = 5
    ps1.channels = [0]
    ov = paramInput.data()
    ov.clear()
    ov.insert( ps1 )
    paramInput.swapBuffers()

    if useTracking:
#        yaw = (0.05 * blockIdx) % (2*np.pi)
#        pitch = 0.1 * np.sin( -0.05 * blockIdx )
        trackingInput.data().orientation = [-az, 0, 0]
        trackingInput.swapBuffers()

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    #print(str(inputBlock.shape))
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-',t, outputSignal[1,:], 'r-')
plt.show()