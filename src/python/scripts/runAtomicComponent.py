# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import sys
sys.path.append( '/home/andi/dev/visr-build-debug/python' )

import visr
import rcl
import rrl

# exec(open("./runAtomicComponent.py").read())

import numpy as np
import matplotlib.pyplot as plt


fs = 48000

blockSize = 128
numBlocks = 16
numSamples = numBlocks*blockSize

t = np.arange(0,numSamples,dtype=np.float32)/fs

inputSignal = np.zeros( [4,numSamples], dtype=np.float32 )
inputSignal[0,:] = np.sin( 2*np.pi*440 * t )
inputSignal[1,:] = 0.5*np.sin( 2*np.pi*880 * t )
inputSignal[2,:] = 0.15*np.sin( 2*np.pi*1340 * t )

referenceOutput = inputSignal[0:2,:] + inputSignal[2:,:] 

outputSignal = np.zeros( (2, numSamples), dtype = np.float32 )

c = visr.SignalFlowContext(blockSize, fs )
 
adder = rcl.Add( c, 'add', None )
 
adder.setup( numInputs = 2, width=2)
 
flow = rrl.AudioSignalFlow( adder )

for blockIdx in range(0,numBlocks):
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

plt.figure(1)
plt.plot( t, referenceOutput[0,:], 'bo-', t, outputSignal[0,:], 'rx-' )
plt.show()
