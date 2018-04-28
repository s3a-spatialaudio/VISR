# %BST_LICENCE_TEXT%

from visr_bst import ObjectToVirtualLoudspeakerRenderer
from visr_bst.util.read_sofa_file import readSofaFile
from visr_bst.util.rotation_functions import sph2cart

import visr
import rrl
import panning
import objectmodel as om

import os
import numpy as np
import matplotlib.pyplot as plt
from urllib.request import urlretrieve # Load SOFA files on the fly
from time import time


############ CONFIG ###############
fs = 48000
blockSize = 1024
parameterUpdatePeriod = 1
numBlocks = 512;
BRIRtruncationLength = None

useTracking = True
useDynamicITD = False
useHRIRinterpolation = True
useCrossfading = True

# Whether the sound source is moving or not
useSourceMovement = False

###################################

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

sofaDirectory = './data'

sofaFile = 'bbcrdlr_systemD.sofa'

fullSofaPath = os.path.join( sofaDirectory, sofaFile )

if not os.path.exists( fullSofaPath ):
    if not os.path.exists( sofaDirectory ):
        os.mkdir( sofaDirectory )
    urlretrieve( 'http://data.bbcarp.org.uk/bbcrd-brirs/sofa/' + sofaFile,
                fullSofaPath )

hrirPos, hrirData, hrirDelays = readSofaFile( fullSofaPath,
                                              truncationLength = BRIRtruncationLength )

arrayConfigFile = 'bs2051-4+5+0.xml'
arrayConfigPath = os.path.join( sofaDirectory, arrayConfigFile )

arrayConfig = panning.LoudspeakerArray( arrayConfigPath )

numObjects = 16

# Dimension of hrirData is #measurement positions x #ears x # lsp x ir length
numLoudspeakers = hrirData.shape[2]

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

renderer = ObjectToVirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", None,
                                      numberOfObjects = numObjects,
                                      hrirPositions = hrirPos,
                                      hrirData = hrirData,
                                      hrirDelays = hrirDelays,
                                      headOrientation = [0.0, 0.0, 0.0],
                                      headTracking = useTracking,
                                      dynamicITD = useDynamicITD,
                                      hrirInterpolation = useHRIRinterpolation,
                                      filterCrossfading = useCrossfading,
                                      loudspeakerConfiguration = arrayConfigPath
                                      )

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )

numOutputChannels = 2;

paramInput = flow.parameterReceivePort('objectVector')
if useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )


inputSignal = np.zeros( (numObjects, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

ov = paramInput.data()


for idx in range(numObjects):
    az = 0
    el = 0
    r = 1
    ps = om.PointSource(idx)
    ps.position = sph2cart( np.asarray([az, el, r]) )
    ps.level = 0.5
    ps.groupId = 5
    ps.priority = 5
    ps.channels = [idx]
    ov.insert( ps )

paramInput.swapBuffers()

start = time()

for blockIdx in range(0,numBlocks):
    if useSourceMovement:
        az = azSequence[int(blockIdx%numPos)]
        el = 0
        ps.position = sph2cart( np.asarray([az, el, r]) )
        ov = paramInput.data()
        ov.set( [ ps ] )
        paramInput.swapBuffers()

    if useTracking:
          headrotation =  azSequence[int(blockIdx%numPos)]
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock

print("fs: %d\t #obj: %d\t #blocks: %d\t blocksize: %d\t expected:%f sec.\t\t Got %f sec"%(fs,numObjects,numBlocks,blockSize,(numBlocks*blockSize)/fs,(time()-start)))
plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-',t, outputSignal[1,:], 'ro-')
plt.show()