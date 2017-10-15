#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 09 2017

@author: Andreas Franck
"""

from hoa_binaural_renderer import HoaBinauralRenderer
from dynamic_binaural_renderer import DynamicBinauralRenderer
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

numBinauralObjects = 64

# datasets are provided for odd orders 1,3,5,7,9
maxHoaOrder = 9

blockSize = 512

numBlocks = 1 + 16; # First block to avoid initial transients, rest for improving the frequency resolution.
#numBlocks = 64;
signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)
numOutputChannels = 2;
#numOutputChannels = (maxHoaOrder+1)**2

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
# inputSignal[0,0:-1:blockSize] = 1.0 # Dirac pulse train
inputSignal[0,blockSize] = 1.0      # Single Dirac impulse

currDir = os.getcwd()
sofaFile = os.path.join( currDir, './data/bbc_hoa2bin_sofa/Gauss_O%d_ku100_dualband_energy.sofa' % maxHoaOrder )

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

azimuths = np.arange( 0, 2*np.pi, 2*np.pi/72, dtype = np.float32 )
elevations = np.ones( azimuths.shape, dtype = np.float32 ) * 0.0

numTestSignals = azimuths.size

HRIRs = np.zeros( (numTestSignals, 2, blockSize*(numBlocks-1) ), dtype = np.float32 )


for testIdx in range(0,numTestSignals):

#    graph = HoaBinauralRenderer( context, "HoaBinauralRenderer", None,
#                                numberOfObjects = numBinauralObjects,
#                                maxHoaOrder = maxHoaOrder,
#                                sofaFile = sofaFile,
#                                interpolationSteps = blockSize,
#                                headTracking = False
#                                )
    graph = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", None, numBinauralObjects,
                                      headTracking = False,
                                      dynITD = False,
                                      dynILD = False,
                                      hrirInterp = True )
    
    flow = rrl.AudioSignalFlow( graph )
    
    paramInput = flow.parameterReceivePort('objectVector')

    outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

    x,y,z = sph2cart( azimuths[testIdx], elevations[testIdx], 1.0 )
    ps1 = objectmodel.PointSource(0)
    ps1.position = [x,y,z]
    ps1.level = 1
    ps1.groupId = 5
    ps1.priority = 5
    ps1.channels = [0]
    ov = paramInput.data()
    ov.clear()
    ov.insert( ps1 )
    paramInput.swapBuffers()


    for blockIdx in range(0,numBlocks):

        inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
        outputBlock = flow.process( inputBlock )
        outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

    HRIRs[testIdx,:,:] = outputSignal[:, blockSize:]


# Filter analysis

H = np.fft.rfft( HRIRs, axis = - 1 )

fGrid = np.arange(0, H.shape[-1]) * fs/HRIRs.shape[-1]
fGridAngular = np.arange(0, H.shape[-1]) * 2*np.pi/HRIRs.shape[-1]

Hmag = np.abs( H )
Hphase = np.angle( H )
Hunwrapped = np.unwrap( Hphase, axis = -1 )

# Phase delay in samples
phaseDelay = Hunwrapped / np.tile( fGridAngular, Hmag.shape[0:2]+(1,) )

IPD = (phaseDelay[:,0,:] - phaseDelay[:,1,:]) / fs


