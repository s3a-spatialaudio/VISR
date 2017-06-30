# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import sys
sys.path.append( '/home/andi/dev/visr-build-debug/python' )
sys.path.append( '/home/andi/dev/visr/src/python/scripts' )

import visr
import rrl
import audiointerfaces as ai

# Usage in debugger:
# exec(open("./pythonAtomicFlowRealTime.py").read())

import numpy as np
import matplotlib.pyplot as plt

import pythonAtoms

fs = 48000

blockSize = 1024

c = visr.SignalFlowContext(blockSize, fs )

# Instantiate a single Python adder
pa = pythonAtoms.PythonAdder3( c, "pa0", None, 2 )

# Or a more configurable Python adder
# pa = PythonAdder( c, "pa0", None, 3, 2 )

#numBlocks = 16
#numSamples = numBlocks*blockSize
#
#t = np.arange(0,numSamples,dtype=np.float32)/fs

#inputSignal = np.zeros( [6,numSamples], dtype=np.float32 )
#inputSignal[0,:] = np.sin( 2*np.pi*440 * t )
#inputSignal[1,:] = 0.5*np.sin( 2*np.pi*880 * t )
#inputSignal[2,:] = 0.15*np.sin( 2*np.pi*1340 * t )

#referenceOutput = inputSignal[0:2,:] + inputSignal[2:4,:] + inputSignal[4:6,:] 
#
#outputSignal = np.zeros( (2, numSamples), dtype = np.float32 )
 
 
flow = rrl.AudioSignalFlow( pa )

aiConfig = ai.AudioInterface.Configuration( flow.numberOfCaptureChannels,
                                           flow.numberOfPlaybackChannels,
                                           fs,
                                           blockSize )

#jackCfg = """{ "clientname": "PythonAdder",
#  "autoconnect" : "true",
#  "portconfig":
#  {
#    "capture":  [{ "externalport" : {"indices": "1:6"} }],
#    "playback": [{ "externalport" : {"indices": "1:2"} }]
#  }
#}"""
 
#jackCfg = """{ "clientname": "PythonAdder",
#  "autoconnect" : "true",
#  "portconfig":
#  {
#    "capture":  [{ "basename":"in", "externalport" : {"portname":"playback_"} }],
#    "playback": [{ "basename":"out", "externalport" : {"portname":"capture_"} }]
#  }
#}"""

jackCfg = """{ "clientname": "PythonAdder",
  "autoconnect" : "false",
  "portconfig":
  {
    "capture":  [{ "basename":"in_", "externalport" : {} }],
    "playback": [{ "basename":"out_", "externalport" : {} }]
  }
}"""
                                           
aIfc = ai.AudioInterfaceFactory.create("Jack", aiConfig, jackCfg)

aIfc.registerCallback( flow )

aIfc.start()

print( "Rendering started." )

#for blockIdx in range(0,numBlocks):
#    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
#    outputBlock = flow.process( inputBlock )
#    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock
#
#plt.figure(1)
#plt.plot( t, referenceOutput[0,:], 'bo-', t, outputSignal[0,:], 'rx-' )
#plt.show()
