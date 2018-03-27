# %BST_LICENCE_TEXT%

import visr
import rrl
import objectmodel

from visr_bst.dynamic_hrir_renderer import DynamicHrirRenderer
from visr_bst.util.sofa_extract_delay import sofaExtractDelay
from visr_bst.util.rotation_functions import sph2cart

import numpy as np
import matplotlib.pyplot as plt

from urllib.request import urlretrieve
import os
from time import time

############ CONFIG ###############
fs = 48000
blockSize = 1024
numBinauralObjects = 32
numOutputChannels = 2 # Binaural output
parameterUpdatePeriod = 1
numBlocks = 128;

useSourceAutoMovement = False
useTracking = True
useDynamicITD = False
useDynamicILD = False
useHRIRinterpolation = True
useCrossfading = True
useInterpolatingConvolver = False

###################################

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

sofaFile = './data/dtf b_nh169.sofa'
if not os.path.exists( sofaFile ):
    urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',sofaFile )

if useDynamicITD:
    sofaFileTD = './data/dtf b_nh169_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        sofaExtractDelay( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

renderer = DynamicHrirRenderer( context, "DynamicHrirRenderer", None,
                               numberOfObjects = numBinauralObjects,
                               sofaFile = sofaFile,
                               headTracking = useTracking,
                               dynamicITD = useDynamicITD,
                               dynamicILD = useDynamicILD,
                               hrirInterpolation = useHRIRinterpolation,
                               filterCrossfading = useCrossfading,
                               interpolatingConvolver = useInterpolatingConvolver
                               )

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )
paramInput = flow.parameterReceivePort('objectVector')

if useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )

inputSignal = np.zeros( (numBinauralObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

az = 0
el = 0
r = 1
ps1 = objectmodel.PointSource(0)
ps1.position = sph2cart( np.array([az, el, r]) )
ps1.level = 0.005
ps1.channels = [ps1.objectId]

pw1 = objectmodel.PlaneWave(1)
pw1.azimuth = az
pw1.elevation = el
pw1.referenceDistance = r
pw1.level = 0.005
pw1.groupId = 5
pw1.priority = 5
pw1.channels = [pw1.objectId]

ov = paramInput.data()
ov.set( [ps1,pw1] )
paramInput.swapBuffers()

tStart = time()

for blockIdx in range(0,numBlocks):
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        if useSourceAutoMovement:
            az = azSequence[int(blockIdx%numPos)]
            el = 0
            ps1.position = sph2cart( np.array([az, el, r]) )
            pw1.azimuth = az
            pw1.elevation = el
            pw1.referenceDistance = r
            ov = paramInput.data()
            ov.set( [ps1, pw1] )
            paramInput.swapBuffers()
        if useTracking:
          headrotation =  azSequence[int(blockIdx%numPos)]
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

print( 'Time elapsed: %f s' % (time()-tStart) )

plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'ro-')
plt.show()
