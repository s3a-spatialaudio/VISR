# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

#exec(open("/home/andi/dev/visr/src/python/scripts/instantiateCoreRenderer.py").read())


import visr
import rcl
import panning
import pml
import rrl
import objectmodel as om

import numpy as np;
import matplotlib.pyplot as plt

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

hfLfPanning = True

blockSize = 128
samplingFrequency = 48000

azGrid = np.arange( -180, 180, 1 ) * np.pi/180.0
gridSize = len( azGrid )
                  
numSamples = 1024;
numObjectChannels = 36;

ctxt = visr.SignalFlowContext( blockSize, samplingFrequency)

regArray = panning.LoudspeakerArray( 'c:/local/visr/src/libpanning/test/matlab/arrays/t-design_t8_P40.xml' )

realArray = panning.LoudspeakerArray( 'c:/local/visr/config/generic/octahedron.xml' )

decodeMatrixFile = 'c:/local/visr/src/libpanning/test/matlab/arrays/decode_N8_P40_t-design_t8_P40.txt'
decodeMatrix = pml.MatrixParameterFloat.fromTextFile( decodeMatrixFile )
    
numSpeakers = realArray.numberOfRegularLoudspeakers

calc = rcl.HoaAllradGainCalculator( ctxt, 'calc', None )
calc.setup( numberOfObjectChannels=numObjectChannels,
            regularArrayConfig=regArray,
            realArrayConfig=realArray,
            decodeMatrix=decodeMatrix )

flow = rrl.AudioSignalFlow( calc )


paramInput = flow.parameterReceivePort('objectInput')
matrixIn = flow.parameterReceivePort('gainInput')
matrixOut = flow.parameterSendPort('gainOutput')

hoaOrder = 3

hoaSource = om.HoaSource(12)
hoaSource.channels = range(0, (hoaOrder+1)**2 )

# Dummy input required for the process() function
inputBlock = np.zeros( (0, blockSize ), dtype=np.float32 )

gains = np.zeros( (gridSize, numSpeakers ) )

for blockIdx in range(0,gridSize):
        
    ov = paramInput.data()
    ov.clear()
    ov.set( hoaSource.objectId, hoaSource )
    paramInput.swapBuffers()
        
    outputBlock = flow.process( inputBlock )

    gains = np.array( matrixOut.data(), copy=True )

#plt.figure(1)
#plt.subplot(121)
#plt.plot( azGrid *180/np.pi, hfGains )
#plt.title( 'HF gains' )
#plt.xlabel( 'azimuth (deg)' )
#plt.subplot(122)
#plt.plot( azGrid *180/np.pi, lfGains )
#axes = plt.gca()
#axes.set_ylim([0,0.9])
#plt.title( 'LF gains' )
#plt.xlabel( 'azimuth (deg)' )
#axes = plt.gca()
#axes.set_ylim([0,0.9])
#plt.show()
