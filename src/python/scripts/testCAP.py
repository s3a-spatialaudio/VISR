#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Feb 20 15:32:08 2018

@author: rdmg1u13
"""

import os
from rcl import CAPGainCalculator
import visr
import panning
import pml

import objectmodel
import rrl

import numpy as np

from urllib.request import urlretrieve
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
lc = panning.LoudspeakerArray( '/Users/rdmg1u13/Google Drive/_dylan/work/soton/_s3a/research/S3A_renderer/VISR_CAPproto/VISR/config/generic/bs2051-0+2+0_90.xml' )

numSpeakers = lc.numberOfRegularLoudspeakers
calc = CAPGainCalculator( ctxt, "CAPCalculator", None,
                  numberOfObjects=numObjectChannels,
                  arrayConfig=lc
                  )

flow = rrl.AudioSignalFlow( calc )


paramInput = flow.parameterReceivePort('objectVectorInput')

# Dummy input required for the process() function
inputBlock = np.zeros( (0, blockSize ), dtype=np.float32 )

gainOutput = flow.parameterSendPort("gainOutput")
if hfLfPanning:
    lfGainOutput = flow.parameterSendPort("lowFrequencyGainOutput")


hfGains = np.zeros( (gridSize, numSpeakers ) )
lfGains = np.zeros( (gridSize, numSpeakers ) )

for blockIdx in range(0,gridSize):
    az = azGrid[blockIdx]
    el = 15.0*np.pi/180.0
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

    hg = np.array( gainOutput.data(), copy=True )
    hfGains[blockIdx,:] = np.ravel( hg )
    if hfLfPanning:
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
