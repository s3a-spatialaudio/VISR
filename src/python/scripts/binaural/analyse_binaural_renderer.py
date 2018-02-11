#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 09 2017

@author: Andreas Franck
"""

from hoa_binaural_renderer import HoaBinauralRenderer
from dynamic_binaural_renderer import DynamicBinauralRenderer
from rotationFunctions import deg2rad, rad2deg
import visr
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

import os

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

# switch dynamic tracking on and off.
useTracking = True


fs = 48000

numBinauralObjects = 1

# datasets are provided for odd orders 1,3,5,7,9
maxHoaOrder = 1

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

azimuths = np.arange( 0, 2*np.pi, 2*np.pi/360, dtype = np.float32 )
elevations = np.ones( azimuths.shape, dtype = np.float32 ) * 0.0

numTestSignals = azimuths.size

HRIRsHoa = np.zeros( (numTestSignals, 2, blockSize*(numBlocks-1) ), dtype = np.float32 )
HRIRsDyn = np.zeros( (numTestSignals, 2, blockSize*(numBlocks-1) ), dtype = np.float32 )

graphHoa = HoaBinauralRenderer( context, "HoaBinauralRenderer", None,
                            numberOfObjects = numBinauralObjects,
                            maxHoaOrder = maxHoaOrder,
                            sofaFile = sofaFile,
                            interpolationSteps = blockSize,
                            headTracking = False
                            )
graphDyn = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", None, numBinauralObjects,
                                  headTracking = False,
                                  dynITD = False,
                                  dynILD = False,
                                  hrirInterp = True )

#graphHoa = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", None, numBinauralObjects,
#                                  headTracking = False,
#                                  dynITD = False,
#                                  dynILD = False,
#                                  hrirInterp = False )

flowHoa = rrl.AudioSignalFlow( graphHoa )
flowDyn = rrl.AudioSignalFlow( graphDyn )

paramInputHoa = flowHoa.parameterReceivePort('objectVector')
paramInputDyn = flowDyn.parameterReceivePort('objectVector')

for testIdx in range(0,numTestSignals):

    outputSignalHoa = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )
    outputSignalDyn = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

    x,y,z = sph2cart( azimuths[testIdx], elevations[testIdx], 1.0 )
    ps1 = objectmodel.PointSource(0)
    ps1.position = [x,y,z]
    ps1.level = 1
    ps1.groupId = 5
    ps1.priority = 5
    ps1.channels = [0]

    ovHoa = paramInputHoa.data()
    ovHoa.clear()
    ovHoa.insert( ps1 )
    paramInputHoa.swapBuffers()

    ovHoa = paramInputDyn.data()
    ovHoa.clear()
    ovHoa.insert( ps1 )
    paramInputDyn.swapBuffers()

    for blockIdx in range(0,numBlocks):

        inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
        outputSignalHoa[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = flowHoa.process( inputBlock )
        outputSignalDyn[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = flowDyn.process( inputBlock )

    HRIRsDyn[testIdx,:,:] = outputSignalDyn[:, blockSize:]
    HRIRsHoa[testIdx,:,:] = outputSignalHoa[:, blockSize:]


# Filter analysis

HDyn = np.fft.rfft( HRIRsDyn, axis = - 1 )
HHoa = np.fft.rfft( HRIRsHoa, axis = - 1 )

fGrid = np.arange(0, HDyn.shape[-1]) * fs/HRIRsDyn.shape[-1]
fGridAngular = np.arange(0, HDyn.shape[-1]) * 2*np.pi/HRIRsDyn.shape[-1]

HmagDyn = np.abs( HDyn )
HphaseDyn = np.angle( HDyn )
HunwrappedDyn = np.unwrap( HphaseDyn, axis = -1 )

# Phase delay in samples
phaseDelayDyn = HunwrappedDyn / np.tile( fGridAngular, HmagDyn.shape[0:2]+(1,) )

IPDDyn = (phaseDelayDyn[:,0,:] - phaseDelayDyn[:,1,:]) / fs

ILDDyn = 20*np.log10(HmagDyn[:,1,:]) - 20*np.log10(HmagDyn[:,0,:])

HmagHoa = np.abs( HHoa )
HphaseHoa = np.angle( HHoa )
HunwrappedHoa = np.unwrap( HphaseHoa, axis = -1 )

# Phase delay in samples
phaseDelayHoa = HunwrappedHoa / np.tile( fGridAngular, HmagHoa.shape[0:2]+(1,) )

IPDHoa = (phaseDelayHoa[:,0,:] - phaseDelayHoa[:,1,:]) / float(fs)

ILDHoa = 20*np.log10(HmagHoa[:,1,:]) - 20*np.log10(HmagHoa[:,0,:])

