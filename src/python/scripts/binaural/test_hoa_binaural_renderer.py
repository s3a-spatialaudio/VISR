#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Oct 09 2017

@author: Andreas Franck
"""

from hoa_binaural_renderer import HoaBinauralRenderer
from hoa_binaural_renderer_serial import HoaBinauralRendererSerial
import visr
import rrl
import objectmodel
import time
import numpy as np
import matplotlib.pyplot as plt

import os

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

############ CONFIG ###############  
fs = 48000
blockSize = 128
numBinauralObjects = 1
numOutputChannels = 2;
parameterUpdatePeriod = 1
numBlocks = 720;
# datasets are provided for odd orders 1,3,5,7,9
maxHoaOrder = 3

useSourceAutoMovement = False
# switch dynamic tracking on and off.
useTracking = True
useSerialPort = True
###################################



signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)
numOutputChannels = 2;
#numOutputChannels = (maxHoaOrder+1)**2

currDir = os.getcwd()
sofaFile = os.path.join( currDir, './data/bbc_hoa2bin_sofa/Gauss_O%d_ku100_dualband_energy.sofa' % maxHoaOrder )

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)
if useSerialPort:
    port = "/dev/cu.usbserial-AJ03GSC8"
    baud = 57600
    graph = HoaBinauralRendererSerial( context, "HoaBinauralRendererSerial", None,
                            numBinauralObjects,
                            port,
                            baud,
                            maxHoaOrder,
                            sofaFile,
                            interpolationSteps = blockSize,
                            headTracking = useTracking
                            )
else:
    graph = HoaBinauralRenderer( context, "HoaBinauralRenderer", None,
                            numBinauralObjects,
                            maxHoaOrder,
                            sofaFile,
                            interpolationSteps = blockSize,
                            headTracking = useTracking
                            )

result,messages = rrl.checkConnectionIntegrity(graph)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( graph )
paramInput = flow.parameterReceivePort('objectVector')


if not useSerialPort and useTracking:
    trackingInput = flow.parameterReceivePort( 'headTracking' )



inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

az = 0
el = 0
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

start = time.time()

for blockIdx in range(0,numBlocks):
    if useSourceAutoMovement:
        az = azSequence[int(blockIdx%numPos)]
        el = 0
        x,y,z = sph2cart( az, el, r )
        ps1.x = x
        ps1.y = y
        ps1.z = z
        ov = paramInput.data()  
        ov.clear()
        ov.insert( ps1 )
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
                 
print("numblocks %d blocksize %d expected:%f sec. Got %f sec"%(numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-',t, outputSignal[1,:], 'r-')
plt.show()