# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

# exec(open("/home/andi/dev/visr/src/python/scripts/instantiate_multi_renderer.py").read())

import visr
import pml
import panning
import objectmodel as om
import rrl

# from multi_renderer import OutputSwitch
from multi_renderer import MultiRenderer
from realtime_multi_renderer import RealTimeMultiRenderer

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os.path

visrBaseDirectory = '/home/andi/dev/visr'

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z


fs = 48000
blockSize = 64

parameterUpdatePeriod = 1024

numBlocks = 1024;

signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)


ctxt = visr.SignalFlowContext(blockSize, fs )

numberOfObjects = 2
numberOfOutputs = 41

lc1 = panning.LoudspeakerArray( os.path.join( visrBaseDirectory, 'config/isvr/audiolab_39speakers_1subwoofer.xml') )
lc2 = panning.LoudspeakerArray( os.path.join( visrBaseDirectory, 'config/isvr/audiolab_22speakers_1subwoofer.xml') )

configFiles = [os.path.join( visrBaseDirectory, 'config/isvr/audiolab_39speakers_1subwoofer.xml'),
                            os.path.join( visrBaseDirectory, 'config/isvr/audiolab_22speakers_1subwoofer.xml')]

#lc1 = panning.LoudspeakerArray( os.path.join( visrBaseDirectory, 'config/generic/bs2051-0+5+0.xml') )
#lc2 = panning.LoudspeakerArray( os.path.join( visrBaseDirectory, 'config/generic/bs2051-0+2+0.xml') )


configs = [lc1, lc2]
# configs = [lc1]

diffFilterFile = os.path.join( visrBaseDirectory, 'config/filters/random_phase_allpass_64ch_128taps.wav')
diffFilters = np.array(pml.MatrixParameterFloat.fromAudioFile( diffFilterFile ))

if True:
    mr = MultiRenderer( ctxt, "MultiRenderer", None,
                   loudspeakerConfigs=configs,
                   numberOfInputs=numberOfObjects,
                   numberOfOutputs=numberOfOutputs,
                   interpolationPeriod=1024,
                   diffusionFilters=diffFilters,
                   controlDataType=pml.UnsignedInteger)
else:
    mr = RealTimeMultiRenderer( ctxt, "MultiRenderer", None,
                   loudspeakerConfigFiles=configFiles,
                   numberOfInputs=numberOfObjects,
                   numberOfOutputs=numberOfOutputs,
                   interpolationPeriod=1024,
                   diffusionFilterFile=diffFilterFile )

flow = rrl.AudioSignalFlow( mr )


objectInput = flow.parameterReceivePort('objectIn')

controlInput = flow.parameterReceivePort('controlIn')

inputSignal = np.zeros( (2, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )


outputSignal = np.zeros( (numberOfOutputs, signalLength ), dtype=np.float32 )

for blockIdx in range(0,numBlocks):
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        az = 0.025 * blockIdx
        el = 0.1 * np.sin( 0.025 * blockIdx )
        r = 1
        x,y,z = sph2cart( az, el, r )
        ps1 = om.PointSource(0)
        ps1.x = x
        ps1.y = y
        ps1.z = z
        ps1.level = 0.5
        ps1.groupId = 5
        ps1.priority = 5
        ps1.resetNumberOfChannels(1)
        ps1.setChannelIndex(0,ps1.objectId)

        ov = objectInput.data()
        # ov.clear()
        ov.insert( ps1 )
        objectInput.swapBuffers()

        renderSelect = int(blockIdx / (parameterUpdatePeriod/blockSize)) % 2

        controlInput.enqueue( pml.UnsignedInteger( renderSelect ) )

    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(1)
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'yx-',  t, outputSignal[2,:], 'gs-', t, outputSignal[3,:], 'bx-', t, outputSignal[4,:], 'm.-' )
plt.show()


#gridX,gridY = np.meshgrid( t, np.arange(0,numberOfOutputs) )
#fig = plt.figure(1)
#ax = fig.add_subplot(111, projection='3d')
#ax.plot_wireframe( gridX, gridY, outputSignal )
#plt.show()

