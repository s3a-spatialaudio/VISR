#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 11:17:34 2017

@author: gc1y17
"""

from virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer
from virtual_loudspeaker_renderer_serial import VirtualLoudspeakerRendererSerial
import visr
import rrl

import numpy as np
import matplotlib.pyplot as plt
from sys import platform
import time


############ CONFIG ###############
fs = 48000
blockSize = 256
numOutputChannels = 2;
parameterUpdatePeriod = 1
numBlocks = 512;
BRIRtruncationLength = 16384

useTracking = True
useDynamicITD = False
useHRIRinterpolation = True
useSerialPort = False
useFilterCrossfading = True
useInterpolatingConvolver = True
fftImplementation = 'ffts'

###################################

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

#sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_reduced_onsets.sofa'
#sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_modelled_onsets_early_dynamic.sofa'
#sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemA.sofa'
sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemB.sofa'


context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

if useSerialPort:
    # TODO: Check and adjust port names for the individual system
    if platform == 'linux' or platform == 'linux2':
        port = "/dev/ttyUSB0"
    elif platform == 'darwin':
        port = "/dev/cu.usbserial-AJ03GSC8"
    elif platform == 'windows':
        port = "COM10"

    # System-specific serial port address
    port = "/dev/ttyUSB0"
    baud = 57600
    renderer = VirtualLoudspeakerRendererSerial( context, "VirtualLoudspeakerRenderer", None,
                                      port,
                                      baud,
                                      sofaFile,
                                      enableSerial = useTracking,
                                      dynITD = useDynamicITD,
                                      hrirInterp = useHRIRinterpolation,
                                      irTruncationLength = BRIRtruncationLength,
                                      filterCrossfading=useFilterCrossfading,
                                      interpolatingConvolver=useInterpolatingConvolver,
                                      fftImplementation=fftImplementation
                                      )
else:
    renderer = VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", None,
                                      sofaFile,
                                      headTracking = useTracking,
                                      dynITD = useDynamicITD,
                                      hrirInterp = useHRIRinterpolation,
                                      irTruncationLength = BRIRtruncationLength,
                                      filterCrossfading=useFilterCrossfading,
                                      interpolatingConvolver=useInterpolatingConvolver,
                                      fftImplementation=fftImplementation
                                      )
#to be completed

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )

if not useSerialPort and useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )

# Determine the channel layout from the audio flow
numLoudspeakers = flow.numberOfCaptureChannels

inputSignal = np.zeros( (numLoudspeakers, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
inputSignal[0,0] = 1
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (0.0*np.pi)/numPos *  np.arange( 0, numPos )
start = time.time()

for blockIdx in range(0,numBlocks):
#   print("NBl "+str(blockIdx))
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        if not useSerialPort and useTracking:
#          headrotation =  np.pi
          headrotation =  azSequence[int(blockIdx%numPos)]
#          print("it num"+str(blockIdx)+" head rotation: "+str(rad2deg(headrotation)))
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock
print("fs: %d\t #obj: %d\t ITD-intrp: %d-%d\t #blocks: %d\t blocksize: %d\t expected:%f sec.\t\t Got %f sec"%(fs,numLoudspeakers,useDynamicITD,useHRIRinterpolation,numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
#print("numblocks %d blocksize %d expected:%f sec. Got %f sec"%(numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'ro-')
plt.show()
