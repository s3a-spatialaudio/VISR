# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr
import pml
import panning
import objectmodel as om
import rrl

# from multi_renderer import OutputSwitch
from multi_renderer import MultiRenderer

import numpy as np
import matplotlib.pyplot as plt

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

lc1 = panning.LoudspeakerArray( 'c:/local/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )
lc2 = panning.LoudspeakerArray( 'c:/local/visr/config/isvr/audiolab_22speakers_1subwoofer.xml' )

configs = [lc1, lc2]

# Slightly convoluted way to get the diffusion filters.
diffFilters = np.array(pml.MatrixParameterFloat.fromAudioFile( 'c:/local/visr/config/filters/random_phase_allpass_64ch_128taps.wav'))

mr = MultiRenderer( ctxt, "MultiRenderer", None,
                   loudspeakerConfigs=configs,
                   numberOfInputs=numberOfObjects,
                   numberOfOutputs=numberOfOutputs,
                   interpolationPeriod=1024,
                   diffusionFilters=diffFilters )


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
        ov.clear()
        ov.set( ps1.objectId, ps1 )
        objectInput.swapBuffers()

        renderSelect = int(blockIdx / (parameterUpdatePeriod/blockSize)) % 2
        
        controlInput.enqueue( pml.UnsignedInteger( renderSelect ) )
        
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(1)
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'rx-' )
plt.show()
