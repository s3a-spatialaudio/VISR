     # -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

#exec(open("/home/andi/dev/visr/src/python/scripts/instantiateCoreRenderer.py").read())


import visr
import signalflows
import panning
import pml
import rrl
import objectmodel

import numpy as np;
import matplotlib.pyplot as plt

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

blockSize = 128
samplingFrequency = 48000
parameterUpdatePeriod = 1024

numBlocks = 1024;





signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength)

numOutputChannels = 2;

ctxt = visr.SignalFlowContext( blockSize, samplingFrequency)

#lc = panning.LoudspeakerArray( '/home/andi/dev/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )
lc = panning.LoudspeakerArray( '/Users/gc1y17/VISR/visr/config/generic/bs2051-0+2+0.xml' )
# lc = panning.LoudspeakerArray( 'c:/local/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )

diffFilters = pml.MatrixParameterFloat( 2, 512, 16 )
#
#renderer1 = signalflows.CoreRenderer( ctxt, 'renderer1', None, lc, 2, numOutputChannels, parameterUpdatePeriod, diffFilters, '' ) 
##                                          loudspeakerConfiguration=lc,
##                                          numberOfInputs=2,
##                                          numberOfOutputs=41, 
##                                          interpolationPeriod=1024, 
##                                          diffusionFilters=diffFilters,
##                                          tra ckingConfiguration='' )

renderer1 = signalflows.CoreRenderer( ctxt, 'renderer1', None,
                                      loudspeakerConfiguration=lc,
                                      numberOfInputs=2,
                                      numberOfOutputs=numOutputChannels, 
                                      interpolationPeriod=parameterUpdatePeriod, 
                                      diffusionFilters=diffFilters,
                                      trackingConfiguration='' )


print( 'Created renderer.' )

flow = rrl.AudioSignalFlow( renderer1 )


paramInput = flow.parameterReceivePort('objectDataInput')

inputSignal = np.zeros( (2, signalLength ), dtype=np.float32 )
inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )


outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

for blockIdx in range(0,numBlocks):
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        az = 0.025 * blockIdx
        el = 0.1 * np.sin( 0.025 * blockIdx )
        r = 1
        x,y,z = sph2cart( az, el, r )
        ps1 = objectmodel.PointSource(0)
        ps1.x = x
        ps1.y = y
        ps1.z = z
        ps1.level = 0.5
        ps1.groupId = 5
        ps1.priority = 5
        ps1.resetNumberOfChannels(1)
        ps1.setChannelIndex(0,ps1.objectId)
        
        ov = paramInput.data()
        ov.clear()
        ov.set( ps1.objectId, ps1 )
        paramInput.swapBuffers()
        
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(1)
plt.plot( t, outputSignal[0,:], 'bo-', t, outputSignal[1,:], 'rx-' )
plt.show()

