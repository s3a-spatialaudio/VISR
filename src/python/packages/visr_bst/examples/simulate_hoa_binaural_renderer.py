#!/usr/bin/env python3

# %BST_LICENCE_TEXT%

import matplotlib.pyplot as plt
import numpy as np

import visr
import rrl
import time

from visr_bst import HoaBinauralRenderer

# %% Configuration
fs = 48000
blockSize = 512
numOutputChannels = 2;     # Binaural
parameterUpdatePeriod = 1
numBlocks = 256;
fftImplementation = 'default' #

# datasets are provided for odd orders 1,3,5,7,9
maxHoaOrder = 3

inputWidth = (maxHoaOrder+1)**2


# TODO: set file location
sofaFile = 'c:/local/SOFA/bbc_hoa2bin/Gauss_O%d_ku100_dualband_energy.sofa' % maxHoaOrder

# Whether the sound source is moving or not
useSourceMovement = True

# switch dynamic tracking on and off.
useTracking = True

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

# The parameter hoaOrder is sensed from the data dimension in the SOFA file.
graph = HoaBinauralRenderer( context, "HoaBinauralRenderer", None,
                     sofaFile=sofaFile,
                     headTracking = useTracking,
                     headOrientation = [np.pi/4, 0.0 ],
                     fftImplementation = fftImplementation
                     )

result,messages = rrl.checkConnectionIntegrity(graph)
if not result:
   raise RuntimeError( "Error in signal flow: %s" % messages)

flow = rrl.AudioSignalFlow( graph )

if useTracking:
    trackingInput = flow.parameterReceivePort( 'tracking' )

inputSignal = np.zeros( (inputWidth, signalLength ), dtype=np.float32 )
inputSignal[2,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

start = time.time()

for blockIdx in range(0,numBlocks):
    if useTracking:
          headrotation =  azSequence[int(blockIdx%numPos)]
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

print("fs: %d\ order: %d\t #blocks: %d\t blocksize: %d\t expected:%f sec.\t\t Got %f sec"%(fs,maxHoaOrder,numBlocks,blockSize,(numBlocks*blockSize)/fs,(time.time()-start)))
plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-',t, outputSignal[1,:], 'ro-')
plt.show()