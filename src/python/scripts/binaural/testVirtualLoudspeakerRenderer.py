#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Oct 27 11:17:34 2017

@author: gc1y17
"""


import time
from extractDelayInSofaFile import extractDelayInSofaFile
from urllib.request import urlretrieve
import os
from virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer
from virtual_loudspeaker_renderer_serial import VirtualLoudspeakerRendererSerial
import visr
import rrl

import numpy as np
import matplotlib.pyplot as plt

############ CONFIG ###############
fs = 48000
blockSize = 512
numLoudspeakers = 12
numOutputChannels = 2;
parameterUpdatePeriod = 1
numBlocks = 512;
BRIRtruncationLength = 4096

useTracking = True
useDynamicITD = False
useHRIRinterpolation = True
useSerialPort = False


###################################

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

sofaFile = './data/SBSBRIR_x0y0.sofa'
if not os.path.exists( sofaFile ):
    urlretrieve( 'http://data.bbcarp.org.uk/sbsbrir/sofa/SBSBRIR_x0y0.sofa',sofaFile )

if useDynamicITD:
    sofaFileTD = './data/SBSBRIR_x0y0_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        extractDelayInSofaFile( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD


context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

if useSerialPort:
    # System-specific serial port address
    port = "/dev/cu.usbserial-AJ03GSC8"
    baud = 57600
    renderer = VirtualLoudspeakerRendererSerial( context, "VirtualLoudspeakerRenderer", None,
                                      numLoudspeakers,
                                      port,
                                      baud,
                                      sofaFile,
                                      enableSerial = useTracking,
                                      dynITD = useDynamicITD,
                                      hrirInterp = useHRIRinterpolation,
                                      irTruncationLength = BRIRtruncationLength
                                      )
else:
    renderer = VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", None,
                                      numLoudspeakers,
                                      sofaFile,
                                      headTracking = useTracking,
                                      dynITD = useDynamicITD,
                                      hrirInterp = useHRIRinterpolation,
                                      irTruncationLength = BRIRtruncationLength
                                      )
#to be completed

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )

if not useSerialPort and useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )

inputSignal = np.zeros( (numLoudspeakers, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
# inputSignal[0,0] = 1
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )
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