#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 14 14:55:25 2017

@author: gc1y17
"""

from rotationFunctions import sph2cart, cart2sph, rad2deg, deg2rad
import wave
import array
from struct import pack
from scipy.io.wavfile import write
import time

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

from dynamic_binaural_renderer import DynamicBinauralRenderer

import random
import visr
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt

fs = 48000
bufferSize = 512

numBinauralObjects = 4
idMatrix = np.identity(3)

blockSize = 128
samplingFrequency = 48000
parameterUpdatePeriod = 1
numBlocks = 1024;
#numBlocks = 64;
signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength)
numOutputChannels = 2;

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

controller = DynamicBinauralRenderer( context, "Controller", None, numBinauralObjects)
#to be completed

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( controller )

headTrackEnabled = True

if headTrackEnabled:
    trackingInput = flow.parameterReceivePort( "tracking" )

paramInput = flow.parameterReceivePort('objectVector')

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
#print(str(inputSignal.shape))

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )
numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )
start = time.time()
for blockIdx in range(0,numBlocks):
#    print("NBl "+str(blockIdx))
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        #az = 0.025 * blockIdx
        #el = 0.1 * np.sin( 0.025 * blockIdx )
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
        if headTrackEnabled:
          headrotation =  np.pi#azSequence[blockIdx%numPos];
          print("it num"+str(blockIdx)+" head rotation: "+str(rad2deg(azSequence[blockIdx%numPos])))
   
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()      
          
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    #print(str(inputBlock.shape))
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock
print("numblocks %d blocksize %d expected:%f sec. Got %f sec"%(numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
#write('testR.wav', 48000, outputSignal[1,:])
plt.figure(random.randint(1, 1000))
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'ro-')
plt.show()