# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

#exec(open("/home/andi/dev/visr/src/python/scripts/testPanningCalculator.py").read())


import visr
import rcl
import panning
import pml
import rrl
import objectmodel

from python_panner import PythonPanner

import numpy as np;
import matplotlib.pyplot as plt

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

hfLfPanning = False

blockSize = 128
samplingFrequency = 48000

azGrid = np.arange( -45, 45, 1 ) * np.pi/180.0
gridSize = len( azGrid )

numSamples = 1024;
numObjectChannels = 1;

ctxt = visr.SignalFlowContext( blockSize, samplingFrequency)

# lc = panning.LoudspeakerArray( '/home/andi/dev/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )
# lc = panning.LoudspeakerArray( '/home/andi/dev/visr/config/generic/bs2051-9+10+3_linear.xml' )
# lc = panning.LoudspeakerArray( 'c:/local/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )
# lc = panning.LoudspeakerArray( 'c:/local/visr/config/generic/bs2051-9+10+3.xml' )
lc = panning.LoudspeakerArray( 'c:/local/s3a_git/aes2018_dualband_panning/code/data/bs2051-4+5+0.xml' )

numSpeakers = lc.numberOfRegularLoudspeakers

if False:
    calc = PythonPanner( ctxt, 'calc', None,
                        numberOfObjects=numObjectChannels,
                        arrayConfig=lc )
else:
    calc = rcl.PanningCalculator( ctxt, 'calc', None,
                                  numberOfObjects=numObjectChannels,
                                  arrayConfig=lc,
                                  separateLowpassPanning=True )

flow = rrl.AudioSignalFlow( calc )


paramInput = flow.parameterReceivePort('objectVectorInput')

# Dummy input required for the process() function
inputBlock = np.zeros( (0, blockSize ), dtype=np.float32 )

lfGainOutput = flow.parameterSendPort("vbapGains")
hfGainOutput = flow.parameterSendPort("vbipGains")

hfGains = np.zeros( (gridSize, numSpeakers ) )
lfGains = np.zeros( (gridSize, numSpeakers ) )

for blockIdx in range(0,gridSize):
    az = 50.0*np.pi/180.0 # azGrid[blockIdx]
    el = 10.0*np.pi/180.0
    r = 1
    x,y,z = sph2cart( az, el, r )
    ps1 = objectmodel.PointSource(0)
    ps1.x = x
    ps1.y = y
    ps1.z = z
    ps1.level = 1
    ps1.channels = [ps1.objectId]
    # Optional: Check channel lock feature,
    # ps1.channelLockDistance = 17.50 # maximum distance (in degree)

    ov = paramInput.data()
    ov.clear()
    ov.insert( ps1 )
    paramInput.swapBuffers()

    outputBlock = flow.process( inputBlock )

    hg = np.array( hfGainOutput.data(), copy=True )
    hfGains[blockIdx,:] = np.ravel( hg )
    lg = np.array( lfGainOutput.data(), copy=True )
    lfGains[blockIdx,:] = np.ravel( lg )


plt.figure(1)
plt.subplot(121)
plt.plot( azGrid *180/np.pi, hfGains, 'b.-' )
plt.title( 'HF gains' )
plt.xlabel( 'azimuth (deg)' )
plt.subplot(122)
plt.plot( azGrid *180/np.pi, lfGains )
axes = plt.gca()
axes.set_xlim([-45,45])
axes.set_ylim([0,0.9])
plt.title( 'LF gains' )
plt.xlabel( 'azimuth (deg)' )
axes = plt.gca()
axes.set_xlim([-45,45])
axes.set_ylim([0,0.9])
plt.show()
