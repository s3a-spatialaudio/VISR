#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 14 14:55:25 2017

@author: gc1y17
"""

from rotationFunctions import sph2cart3inp

from dynamic_binaural_renderer import DynamicBinauralRenderer
from dynamic_binaural_renderer_serial import DynamicBinauralRendererSerial
import visr
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt

from sys import platform
import time
from extractDelayInSofaFile import extractDelayInSofaFile
from urllib.request import urlretrieve
import os

############ CONFIG ###############
fs = 48000
blockSize = 1024
numBinauralObjects = 64
numOutputChannels = 2 # Binaural output
parameterUpdatePeriod = 1
numBlocks = 128;

useSourceAutoMovement = False
useTracking = True
useDynamicITD = True
useDynamicILD = False
useHRIRinterpolation = True
useFilterCrossfading = True
useSerialPort = False

###################################

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

sofaFile = './data/dtf b_nh169.sofa'
if not os.path.exists( sofaFile ):
    urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',sofaFile )


if useDynamicITD:
    sofaFileTD = './data/dtf b_nh169_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        extractDelayInSofaFile( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

if useSerialPort:

    # TODO: Check and adjust port names for the individual system
    if platform == 'linux' or platform == 'linux2':
        port = "/dev/ttyUSB1"
    elif platform == 'darwin':
        port = "/dev/cu.usbserial-AJ03GSC8"
    elif platform == 'windows':
        port = "COM10"

    baud = 57600
    renderer = DynamicBinauralRendererSerial( context, "DynamicBinauralRendererSerial", None,
                                           numBinauralObjects,
                                           port,
                                           baud,
                                           sofaFile,
                                           enableSerial = useTracking,
                                           dynITD = useDynamicITD,
                                           dynILD = useDynamicILD,
                                           hrirInterp = useHRIRinterpolation,
                                           headTrackingCalibrationPort = 8889,
                                           filterCrossfading = useFilterCrossfading
                                           )
else:
    renderer = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", None,
                                      numBinauralObjects,
                                      sofaFile,
                                      headTracking = useTracking,
                                      dynITD = useDynamicITD,
                                      dynILD = useDynamicILD,
                                      hrirInterp = useHRIRinterpolation,
                                      filterCrossfading = useFilterCrossfading
                                      )

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )
paramInput = flow.parameterReceivePort('objectVector')

if not useSerialPort and useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

#az = 0.025 * blockIdx
#el = 0.1 * np.sin( 0.025 * blockIdx )
az = 0
el = 0
r = 1
x,y,z = sph2cart3inp( az, el, r )
ps1 = objectmodel.PointSource(0)
ps1.x = x
ps1.y = y
ps1.z = z
ps1.level = 0.5
ps1.groupId = 5
ps1.priority = 5
ps1.resetNumberOfChannels(1)
ps1.setChannelIndex(0,ps1.objectId)

pw1 = objectmodel.PlaneWave(1)
pw1.azimuth = az
pw1.elevation = el
pw1.referenceDistance = r
pw1.level = 0.5
pw1.groupId = 5
pw1.priority = 5
pw1.channels = [pw1.objectId]

ov = paramInput.data()
ov.clear()
ov.insert( [ps1,pw1] )
paramInput.swapBuffers()

start = time.time()

for blockIdx in range(0,numBlocks):
#   print("NBl "+str(blockIdx))
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        if useSourceAutoMovement:
            az = azSequence[int(blockIdx%numPos)]
            el = 0
            x,y,z = sph2cart3inp( az, el, r )
            ps1.x = x
            ps1.y = y
            ps1.z = z
            pw1.azimuth = az
            pw1.elevation = el
            pw1.referenceDistance = r
            ov = paramInput.data()
            ov.clear()
            ov.insert( [ps1, pw1] )
            paramInput.swapBuffers()

        if not useSerialPort and useTracking:
#          headrotation =  np.pi
          headrotation =  azSequence[int(blockIdx%numPos)]
#          print("it num"+str(blockIdx)+" head rotation: "+str(rad2deg(headrotation)))
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock
print("fs: %d\t #obj: %d\t ITD-intrp: %d-%d\t #blocks: %d\t blocksize: %d\t expected:%f sec.\t\t Got %f sec"%(fs,numBinauralObjects,useDynamicITD,useHRIRinterpolation,numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
#print("numblocks %d blocksize %d expected:%f sec. Got %f sec"%(numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'ro-')
plt.show()
