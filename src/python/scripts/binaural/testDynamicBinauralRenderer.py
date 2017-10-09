#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 14 14:55:25 2017

@author: gc1y17
"""

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

from dynamic_binaural_renderer_serial import DynamicBinauralRendererSerial
import random
import visr
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt


fs = 44100

numBinauralObjects = 12
port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600
idMatrix = np.identity(3)

blockSize = 512

parameterUpdatePeriod = 1
numBlocks = 1024;
#numBlocks = 64;
signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)
numOutputChannels = 2;



context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)
controller = DynamicBinauralRendererSerial( context, "Controller", None, numBinauralObjects, port, baud,True,True,False,True)
#to be completed

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( controller )

paramInput = flow.parameterReceivePort('objectDataInput')

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
#print(str(inputSignal.shape))

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
    
        #az = 0.025 * blockIdx
        #el = 0.1 * np.sin( 0.025 * blockIdx )
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    #print(str(inputBlock.shape))
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(1)
plt.plot( t, outputSignal[0,:], 'b-',t, outputSignal[1,:], 'r-')
plt.show()