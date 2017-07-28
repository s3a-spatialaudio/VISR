# -*- coding: utf-8 -*-
"""
Created on Thu Jul 27 16:35:27 2017

@author: af5u13
"""

import visr
import rcl
import rrl
import pml

import numpy as np
import matplotlib.pyplot as plt

#exec(open("time_frequency_filtering.py").read())
#exec(open("/home/andi/dev/visr/src/python/scripts/time_frequency_filtering.py").read())

class TFProcessing( visr.CompositeComponent ):
    def __init__( self, context, name, parent, numberOfChannels, dftLength,
                 hopSize, window=None, windowLength=None, fftImplementation = "default" ):
        super(TFProcessing, self ).__init__( context, name, parent )
        if not (window is None):
            self.ForwardTransform = rcl.TimeFrequencyTransform( context, "FwdTransform", self,
                              numberOfChannels=numberOfChannels,
                              dftLength=dftLength, window=window, hopSize=hopSize,
                              fftImplementation=fftImplementation )
        elif not (windowLength is None):
            self.ForwardTransform = rcl.TimeFrequencyTransform( context, "FwdTransform", self,
                              numberOfChannels=numberOfChannels,
                              dftLength=dftLength, windowLength=windowLength, hopSize=hopSize,
                              fftImplementation=fftImplementation )
        else:
            raise ValueError( "One of the arguments 'window' or 'windowLength' must be given." )
        
        self.InverseTransform = rcl.TimeFrequencyInverseTransform( context, "InverseTransform", self,
                              numberOfChannels=numberOfChannels,
                              dftLength=dftLength, hopSize=hopSize,
                              fftImplementation=fftImplementation )
        self.audioIn = visr.AudioInputFloat( "in", self, numberOfChannels )
        self.audioOut = visr.AudioOutputFloat( "out", self, numberOfChannels )
        self.audioConnection( self.audioIn, self.ForwardTransform.audioPort("in") )
        self.audioConnection( self.InverseTransform.audioPort( "out"), self.audioOut )
        self.parameterConnection( self.ForwardTransform.parameterPort("out"),
                                 self.InverseTransform.parameterPort("in") )

samplingFrequency = 48000
blockSize = 64

numberOfChannels = 2;
dftSize = 2*blockSize
windowSize = 2*blockSize
hopSize = blockSize

context = visr.SignalFlowContext( blockSize, samplingFrequency )

# Design a Hann window that satisfies the COLA (constant overlap-add) property.
tUnity = np.array( np.arange(0,dftSize) )
window = np.sin( np.pi/float(dftSize) * tUnity)**2

#cc = TFProcessing( context, "top", None, numberOfChannels=numberOfChannels,
#                  dftLength=dftSize, window=window, hopSize=hopSize )
cc = TFProcessing( context, "top", None, numberOfChannels=numberOfChannels,
                  dftLength=dftSize, windowLength=windowSize, hopSize=hopSize )

res, msg = rrl.checkConnectionIntegrity( cc )
if not res:
    print( "The top-level composite is inconsistent: %s" % msg )
    # TODO: Abort here

flow = rrl.AudioSignalFlow( cc )

numBlocks = 16
numSamples = numBlocks*blockSize

t = np.arange(0,numSamples,dtype=np.float32)/samplingFrequency

inputSignal = np.zeros( [numberOfChannels,numSamples], dtype=np.float32 )
inputSignal[0,:] = np.sin( 2*np.pi*440 * t )
inputSignal[1,:] = 0.5*np.sin( 2*np.pi*880 * t )

outputSignal = np.zeros( [numberOfChannels,numSamples], dtype=np.float32 )

for blockIdx in range(0,numBlocks):
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

plt.figure(1)
plt.plot( t+float(blockSize)/float(samplingFrequency), inputSignal[0,:], 'bo-', t, outputSignal[0,:], 'rx-' )
plt.show()
