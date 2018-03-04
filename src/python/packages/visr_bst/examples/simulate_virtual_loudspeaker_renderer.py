# %BST_LICENCE_TEXT%

from visr_bst import VirtualLoudspeakerRenderer
from visr_bst.util.read_sofa_file import readSofaFile

import visr
import rrl

import os
import numpy as np
import matplotlib.pyplot as plt
from urllib.request import urlretrieve # Load SOFA files on the fly


############ CONFIG ###############
fs = 48000
blockSize = 1024
parameterUpdatePeriod = 1
numBlocks = 512;
BRIRtruncationLength = None

useTracking = True
useDynamicITD = False
useHRIRinterpolation = True

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

# Dimension of hrirData is #measurement positions x #ears x # lsp x ir length
numLoudspeakers = hrirData.shape[2]

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

renderer = VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", None,
                                      hrirPositions = hrirPos,
                                      hrirData = hrirData,
                                      hrirDelays = hrirDelays,
                                      headOrientation = [0.0, 0.0, 0.0],
                                      headTracking = useTracking,
                                      dynamicITD = useDynamicITD,
                                      hrirInterpolation = useHRIRinterpolation,
                                      )

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )

numOutputChannels = 2;

if useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )

inputSignal = np.zeros( (numLoudspeakers, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

numPos = 360/5
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

for blockIdx in range(0,numBlocks):
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        if useTracking:
          headrotation =  azSequence[int(blockIdx%numPos)]
          trackingInput.data().orientation = [headrotation,0,0] #rotates over the z axis, that means that the rotation is on the xy plane
          trackingInput.swapBuffers()
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure()
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'ro-')
plt.show()