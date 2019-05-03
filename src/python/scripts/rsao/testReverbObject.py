     # -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

# Usage for debugging from raw Python console
#exec(open("/Users/af5u13/dev/visr/src/python/scripts/rsao/testReverbObject.py").read())

import visr
import signalflows
import panning
import pml
import rrl
import objectmodel as om

import numpy as np;
import matplotlib.pyplot as plt
import os

# Get VISR base directory from rsao subdirectory.
visrBaseDirectory = os.path.normpath(os.path.join( os.getcwd(), '../../../..' )).replace('\\','/')

def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

blockSize = 128
samplingFrequency = 48000
parameterUpdatePeriod = 1024

numBlocks = 64;

numChannels = 5;


signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength)

ctxt = visr.SignalFlowContext( blockSize, samplingFrequency)

lspConfigFile  = os.path.join( visrBaseDirectory, 'config/generic/bs2051-9+10+3.xml')
# lspConfigFile  = os.path.join( visrBaseDirectory, 'config/isvr/audiolab_39speakers_1subwoofer.xml' )

lc = panning.LoudspeakerArray( lspConfigFile )

numOutputChannels = np.max( lc.channelIndices() + lc.subwooferChannelIndices() )
numLoudspeakers = lc.numberOfRegularLoudspeakers

diffFilterFile = os.path.join( visrBaseDirectory, 'config/filters/random_phase_allpass_64ch_512taps.wav')
diffFiltersRaw = np.array(pml.MatrixParameterFloat.fromAudioFile( diffFilterFile ),
                          dtype = np.float32 )
diffFilters = pml.BasicMatrixFloat( diffFiltersRaw[ np.array(lc.channelIndices() )-1,: ] )

reverbConfigStr = '{ "numReverbObjects": %i, "discreteReflectionsPerObject": 20, "lateReverbFilterLength": 2.0, "lateReverbDecorrelationFilters": "%s/config/filters/random_phase_allpass_64ch_1024taps.wav" }' % (numChannels, visrBaseDirectory )

renderer1 = signalflows.CoreRenderer( ctxt, 'renderer1', None,
                                      loudspeakerConfiguration=lc,
                                      numberOfInputs=1,
                                      numberOfOutputs=numOutputChannels, 
                                      interpolationPeriod=parameterUpdatePeriod, 
                                      diffusionFilters=diffFilters,
                                      reverbConfig=reverbConfigStr,
                                      trackingConfiguration='' )


print( 'Created renderer.' )

flow = rrl.AudioSignalFlow( renderer1 )

# Create the object vector
jsonFile = os.path.join( visrBaseDirectory, 'src/libobjectmodel/test/data/point_source_with_reverb_1.json' )
obj_vector=om.ObjectVector() # load the object vector
obj_str=open(jsonFile).read() # read json object vector as a string
obj_vector.fillFromJson(obj_str) # populate the object vector
ro = obj_vector[0]
                       
paramInput = flow.parameterReceivePort('objectDataInput')

inputSignal = np.zeros( (1, signalLength ), dtype=np.float32 )
# inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
inputSignal[ 0, 100 ] = 1

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

for blockIdx in range(0,numBlocks):
    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
        ov = paramInput.data()
        ov.clear()
        ov.set(  ro.objectId, ro )
        paramInput.swapBuffers()
        
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(1)
plt.plot( t, outputSignal[2,:], 'bo-', t, outputSignal[6,:], 'rx-',  t, outputSignal[21,:], 'm.-' )
plt.show( block = False )