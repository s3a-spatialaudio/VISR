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

# Usage in debugger:
# exec(open("./pythonAtomicFlow.py").read())

import numpy as np
import matplotlib.pyplot as plt

class PythonComposite( visr.CompositeComponent ):
    def __init__( self, context, name, parent, numInputs, width ):
        super(PythonAdder,self).__init__( context, name, parent )
        self.output = visr.AudioOutput( name, self, width )
        self.width = width
        self.inputs = []
        for inputIdx in range( 0, numInputs ):
            portName =  "in%d" % inputIdx
            inPort = visr.AudioInput( portName, self, width )
            self.inputs.append( inPort )
            
    def process( self ):
        if len( self.inputs ) == 0:
            self.output.set( np.zeros( (self.output.width, self.period ), dtype = np.float32 ) )
        elif len( self.inputs ) == 1:
            self.output.set( np.array(self.inputs[0]))
        else:
            acc = np.array(self.inputs[0])
            for idx in range(1,len(self.inputs)):
                acc += np.array(self.inputs[idx])
            self.output.set( acc )


fs = 48000

blockSize = 64

c = visr.SignalFlowContext(blockSize, fs )

# Instantiate a single Python adder
pa = PythonAdder( c, "pa0", None, 3, 2 )

numBlocks = 16
numSamples = numBlocks*blockSize

t = np.arange(0,numSamples,dtype=np.float32)/fs

inputSignal = np.zeros( [6,numSamples], dtype=np.float32 )
inputSignal[0,:] = np.sin( 2*np.pi*440 * t )
inputSignal[1,:] = 0.5*np.sin( 2*np.pi*880 * t )
inputSignal[2,:] = 0.15*np.sin( 2*np.pi*1340 * t )

referenceOutput = inputSignal[0:2,:] + inputSignal[2:4,:] + inputSignal[4:6,:] 

outputSignal = np.zeros( (2, numSamples), dtype = np.float32 )
 
 
flow = rrl.AudioSignalFlow( pa )

for blockIdx in range(0,numBlocks):
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

plt.figure(1)
plt.plot( t, referenceOutput[0,:], 'bo-', t, outputSignal[0,:], 'rx-' )
plt.show()
