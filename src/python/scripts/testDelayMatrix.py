# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import sys
sys.path.append( '/home/andi/dev/visr-build-debug/python' )

import visr
import pml # This loads the parameter types and protocols used.
import rcl
import rrl

# Usage in debugger:
# exec(open("/home/andi/dev/visr/src/python/scripts/parameterConnection.py").read())

import numpy as np
import matplotlib.pyplot as plt



fs = 48000

blockSize = 16

numberOfInputs = 3
numberOfOutputs = 2

numBlocks = 128
numSamples = numBlocks*blockSize

# Update the parameters every #updateRate iterations.
updateRate = 4

ctxt = visr.SignalFlowContext(blockSize, fs )

# Instantiate the atomic flow
dl = rcl.DelayMatrix( ctxt, "DL" )
dl.setup(numberOfInputs=numberOfInputs,
         numberOfOutputs=numberOfOutputs, 
         interpolationType="lagrangeOrder0", initialDelay=0.0001,
         initialGain=0.0, interpolationSteps=updateRate*blockSize, 
         methodDelayPolicy=rcl.DelayMatrix.MethodDelayPolicy.Limit, 
         controlInputs =rcl.DelayMatrix.ControlPortConfig.All )

flow = rrl.AudioSignalFlow( dl )

gainInputProtocol = flow.parameterReceivePort("gainInput")
delayInputProtocol = flow.parameterReceivePort("delayInput")


t = np.arange(0,numSamples,dtype=np.float32)/fs

inputSignal = np.zeros( [numberOfInputs,numSamples], dtype=np.float32 )
inputSignal[0,:] = 0.5 * np.sin( 2.0*440*np.pi*t )
inputSignal[1,:] = 0.25 * np.cos( 2.0*880*np.pi*t )
inputSignal[2,:] = 0.25 * np.cos( 2.0*1760*np.pi*t )

outputSignal = np.zeros( [numberOfOutputs,numSamples], dtype=np.float32 )

gainInputProtocol.data()[0,0] = 1.0
gainInputProtocol.data()[1,2] = 0.7
gainInputProtocol.data()[1,0] = 0.2


#gainParameter = pml.MatrixParameterFloat( numberOfOutputs, 16 )
#delayParameter = pml.VectorParameterFloat( numberOfInputs, 16 )

#gainParameter = np.zeros( (numberOfOutputs, numberOfInputs), dtype=np.float32 )
#gainParameter[0,0] = 1.0
#gainParameter[0,1] = 1.0
#gainParameter[0,2] = 0.5
#
#mtx = gainInputProtocol.data()
#mtx = gainParameter
gainInputProtocol.swapBuffers()

# delayInputProtocol.data().set( [ 1.4e-3, 1e-3 ] )

#delayInputProtocol.swapBuffers()

#print( gainInputProtocol.data() )
#print( delayInputProtocol.data() )


for blockIdx in range(0,numBlocks):
    # newGains = [0.5+0.5*np.cos(float(blockIdx)*np.pi/4 ), 0.7 ]
    #gainInputProtocol.data().set( newGains )
    # gainInputProtocol.swapBuffers()

    if blockIdx % updateRate == 0:
        delay = [8e-4+8e-4*np.cos(float(blockIdx)*np.pi/4/updateRate ), 1e-3 ]
        delayMtx = delayInputProtocol.data()
        delayMtx[0,0] = 8e-4+8e-4*np.cos(float(blockIdx)*np.pi/4/updateRate )
        delayInputProtocol.swapBuffers()

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

plt.figure(1)
#plt.plot( t*fs, refSignal, 'bo-', t*fs, outputSignal[0,:], 'rx-' ) # ,  t*fs, outputSignal[1,:], 'm.-' )
plt.plot( t*fs, outputSignal[0,:], 'bo-', t*fs, outputSignal[1,:], 'rx-' ) # ,  t*fs, outputSignal[1,:], 'm.-' )
plt.show()

