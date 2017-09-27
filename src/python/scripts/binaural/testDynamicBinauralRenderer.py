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

from dynamic_binaural_renderer import DynamicBinauralRenderer
from serial_reader import serialReader

import random
import visr
import pml
import rrl
import objectmodel

import numpy as np
import matplotlib.pyplot as plt

class DynamicBinauralRendererSerial(visr.CompositeComponent ):    
        def __init__( self,
                     context, name, parent, 
                     numberOfObjects,
                     port,
                     baud
                     ):
            super( DynamicBinauralRendererSerial, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectDataInput", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
            self.dynamicBinauralRenderer = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", self, numBinauralObjects)
            self.serialReader = serialReader(context, "Controller", self,port, baud )

            self.parameterConnection( self.objectVectorInput, self.dynamicBinauralRenderer.parameterPort("objectVector"))
            self.parameterConnection( self.serialReader.parameterPort("orientation"), self.dynamicBinauralRenderer.parameterPort("tracking"))
            self.audioConnection(  self.objectSignalInput, self.dynamicBinauralRenderer.audioPort("audioIn"))
            self.audioConnection( self.dynamicBinauralRenderer.audioPort("audioOut"), self.binauralOutput)


fs = 48000
bufferSize = 256

numBinauralObjects = 12
port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600
idMatrix = np.identity(3)

blockSize = 128
samplingFrequency = 48000
parameterUpdatePeriod = 1024
#numBlocks = 1024;
numBlocks = 64;
signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength)
numOutputChannels = 2;



context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

controller = DynamicBinauralRendererSerial( context, "Controller", None, numBinauralObjects, port, baud)
#to be completed

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( controller )

paramInput = flow.parameterReceivePort('objectDataInput')

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
print(str(inputSignal.shape))

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

for blockIdx in range(0,numBlocks):
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        #az = 0.025 * blockIdx
        #el = 0.1 * np.sin( 0.025 * blockIdx )
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
        
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    #print(str(inputBlock.shape))
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(random.randint(1, 1000))
plt.plot( t, outputSignal[0,:], 'bo-')
plt.show()