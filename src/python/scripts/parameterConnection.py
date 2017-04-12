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
# exec(open("./parameterConnection.py").read())

import numpy as np
import matplotlib.pyplot as plt



fs = 48000

blockSize = 64

numberOfChannels = 2

c = visr.SignalFlowContext(blockSize, fs )

if True:
    cc = visr.CompositeComponent( c, "top" )

    # Instantiate a single delay line
    dl = rcl.DelayVector( c, "DL", cc )
    dl.setup(numberOfChannels, interpolationType=rcl.DelayVector.InterpolationType.Linear, initialDelay=0,
         initialGain=1.0, interpolationSteps=blockSize)

    aip = visr.AudioInputFloat( "globalAudioIn", cc, numberOfChannels )
    aop = visr.AudioOutputFloat( "globalAudioOut", cc, numberOfChannels )

    cc.audioConnection( aip, dl.audioPort("in") )
    cc.audioConnection( dl.audioPort("out"), aop )

    # Define the global parameter inputs and the corresponding connections.
    vecParamType = pml.VectorParameterFloat.staticType
    vecCfg = pml.VectorParameterConfig(numberOfChannels)
    dbProtType = pml.DoubleBufferingProtocol.staticType
    
    topGainInput = visr.ParameterInput( "gainInput", cc, vecParamType, dbProtType, vecCfg )
    topDelayInput = visr.ParameterInput( "delayInput", cc, vecParamType, dbProtType, vecCfg )

    cc.parameterConnection( topGainInput, dl.parameterPort("gainInput"))
    cc.parameterConnection( topDelayInput, dl.parameterPort("delayInput"))

    flow = rrl.AudioSignalFlow( cc )
else:
    # Instantiate the atomic flow
    dl = rcl.DelayVector( c, "DL" )
    dl.setup(numberOfChannels, interpolationType=rcl.DelayVector.InterpolationType.Linear, initialDelay=0,
         initialGain=1.0, interpolationSteps=blockSize)

    flow = rrl.AudioSignalFlow( dl )

gainInputProtocol = flow.parameterReceivePort("gainInput")
delayInputProtocol = flow.parameterReceivePort("delayInput")


numBlocks = 16
numSamples = numBlocks*blockSize

t = np.arange(0,numSamples,dtype=np.float32)/fs

inputSignal = np.zeros( [numberOfChannels,numSamples], dtype=np.float32 )
inputSignal[0,:] = 0.5 * np.sin( 2.0*440*np.pi*t )

outputSignal = np.zeros( [numberOfChannels,numSamples], dtype=np.float32 )

gainParameter = pml.VectorParameterFloat( numberOfChannels, 16 )
delayParameter = pml.VectorParameterFloat( numberOfChannels, 16 )

gainParameter[0] = 1.0
gainParameter[1] = -0.1

delayParameter[0] = 1e-3

print( gainInputProtocol.data() )
print( delayInputProtocol.data() )


gainInputProtocol.data().set( [0.5, 0.25 ])
delayInputProtocol.data().set( [ 1.4e-3, 1e-3 ] )
#gainInputProtocol.swapBuffers()
#delayInputProtocol.swapBuffers()

print( gainInputProtocol.data() )
print( delayInputProtocol.data() )


for blockIdx in range(0,numBlocks):
    newGains = [1.0+0.1*np.cos(float(blockIdx)*np.pi/4 ), 0.7 ]
    gainInputProtocol.data().set( newGains )
    delayInputProtocol.data().set( [ 1e-3, 2e-3 ] )
    gainInputProtocol.swapBuffers()
    delayInputProtocol.swapBuffers()

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

plt.figure(1)
plt.plot( t, inputSignal[0,:], 'bo-', t, outputSignal[0,:], 'rx-' )
plt.show()

