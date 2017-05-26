# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

#exec(open("/home/andi/dev/visr/src/python/scripts/instantiateCoreRenderer.py").read())


import visr
import rcl
import panning
import pml
import rrl
import objectmodel

import numpy as np;
import matplotlib.pyplot as plt

blockSize = 128
samplingFrequency = 48000
   
numBlocks = 20
               
numSamples = numBlocks * blockSize
t = np.arange(0,numSamples)

numChannels = 3;

inputSignal = np.ones( (numChannels, numSamples), dtype=np.float32 )
outputSignal = np.inf*np.ones( (numChannels, numSamples), dtype=np.float32 )

interpolationPeriod = 64

ctxt = visr.SignalFlowContext( blockSize, samplingFrequency)

gvec = rcl.GainVector( ctxt, 'calc', None )
gvec.setup( numberOfChannels=numChannels,
           interpolationSteps = 88,
           controlInputs=True )

flow = rrl.AudioSignalFlow( gvec )


paramInput = flow.parameterReceivePort('gainInput')
gv = paramInput.data()
gv.set( np.asarray([1.0,1.0,1.0]))
paramInput.swapBuffers()

# Dummy input required for the process() function


for blockIdx in range(0,numBlocks):
    
    if blockIdx == 3:
        gv = paramInput.data()
        gv[0] = 0.55
        gv[1] = 1.0
        gv[2] = 1.0
        paramInput.swapBuffers()

    if blockIdx == 6:
        gv = paramInput.data()
        gv[0] = 0.7
        gv[1] = 1.9
        gv[2] = 1.0
        paramInput.swapBuffers()
        
    outputBlock = flow.process( inputSignal[:,blockIdx*blockSize:(blockIdx+1)*blockSize] )

    outputSignal[:,blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock;

plt.figure(1)
plt.plot( t, outputSignal[0,:], 'b.-' )
plt.show()
