# -*- coding: utf-8 -*-
"""
Created on Fri Sep  1 15:22:13 2017

@author: af5u13
"""

# Template for an atomic component that takes in an audio signal and outputs a
# stream of indicator values (meter outputs)

# %% Module imports

# VISR core modules
import visr   # Core VISR module, defines components and ports
import rrl    # VISR runtime library, for running components.

from audio_meter import LoudnessMeter

import numpy as np
import matplotlib.pyplot as plt

# Define basic parameters of the simulated run.
blockSize = 64
numBlocks = 1024
samplingFrequency = 48000

signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength, dtype = np.float32 )

numberOfChannels = 5

channelWeights = np.power( 10.0, np.array( [0.0,0.0,0.0,1.5,1.5] )/20.0 )

# Define the input signal
inputSignal = np.asarray(2.0*np.random.rand( numberOfChannels, signalLength ) - 1.0, dtype=np.float32 )
#inputSignal[0,:] = np.sin( 2.0*np.pi*440 * t )
                            
# Data structure to define basic
context = visr.SignalFlowContext( blockSize, samplingFrequency )

# Instantiate the signal flow (a component)
meter = LoudnessMeter( context, "meter", None, numberOfChannels = 5,
                      channelWeights = channelWeights )

# Instantiate an AudioSignalFlow object to execute the component.
flow = rrl.AudioSignalFlow( meter )

loudnessOut = flow.parameterSendPort( 'loudnessOut' )


# Define an output array
# Replace by a ndarray with an estimate of the size and.
loudnessValues = []

for blockIdx in range(0,numBlocks):
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )

    if not loudnessOut.empty():
        Lk = loudnessOut.front()
        loudnessValues.append( Lk.value )
        loudnessOut.pop()
                 
        
loudness = np.asarray( loudnessValues, dtype=np.float32 )

# Visualise the result
plt.figure(1)
plt.plot( loudness, 'b.-' )
plt.show( block = False)
