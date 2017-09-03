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
import pml    # Parameter message library, defines standard parameter types and communication protocols.
import rrl    # VISR runtime library, for running components.

from audio_meter import SurroundLoudnessMeter

import numpy as np
import matplotlib.pyplot as plt

# Define basic parameters of the simulated run.
blockSize = 64
numBlocks = 1024
samplingFrequency = 48000

signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength, dtype = np.float32 )

numberOfChannels = 5

# Define the input signal
inputSignal = np.asarray(2.0*np.random.rand( numberOfChannels, signalLength ) - 1.0, dtype=np.float32 )
#inputSignal[0,:] = np.sin( 2.0*np.pi*440 * t )

# Preallocate the output signal
outputSignal = np.ones( ( numberOfChannels, signalLength ), dtype = np.float32 )
                            
# Data structure to define basic
context = visr.SignalFlowContext( blockSize, samplingFrequency )

# Instantiate the signal flow (a component)
meter = SurroundLoudnessMeter( context, "meter", None, numberOfChannels = 5, gatingPeriod = None)

# Instantiate an AudioSignalFlow object to execute the component.
flow = rrl.AudioSignalFlow( meter )

loudnessOut = flow.parameterSendPort( 'loudnessOut' )

for blockIdx in range(0,numBlocks):
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

    if not loudnessOut.empty():
        Lk = loudnessOut.front()
        print( "loudness: %f dB" % Lk.value )
        loudnessOut.pop()
                 
# Visualise the result
plt.figure(1)
plt.plot( t, inputSignal[0,:], 'bo-', t, outputSignal[0,:], 'rx-' )
plt.show( block = False)

X = np.fft.rfft( inputSignal )
Y = np.fft.rfft( outputSignal )
fGrid = np.arange( 0, X.shape[1] ) / signalLength * samplingFrequency

plt.figure(2)
plt.semilogx( fGrid, 20.0*np.log10(np.abs(X[0,:])), 'bo-', fGrid, 20.0*np.log10(np.abs(Y[0,:])), 'm.-' )
plt.show( block = False )
                 