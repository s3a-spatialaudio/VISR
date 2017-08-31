     # -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

# Usage for debugging from raw Python console
#exec(open("/Users/af5u13/dev/visr/src/python/scripts/rsao/reverbObjectBinauralisation.py").read())

import visr
import signalflows
import panning
import pml
import rcl
import rrl
#import objectmodel as om

import h5py
import numpy as np;
import matplotlib.pyplot as plt
import os

class ReverbToBinaural( visr.CompositeComponent ):
    def __init__( self, context, name, parent,
                 loudspeakerConfig,
                 numberOfInputs,
                 rendererOutputs,
                 interpolationPeriod, 
                 diffusionFilters,
                 trackingConfiguration,
                 brirRouting,
                 brirFilters,
                 scenePort = 4242,
                 reverbConfiguration='',
                 skipConvolver = False):
        super(ReverbToBinaural,self).__init__( context, name, parent )
        self.coreRenderer = signalflows.BaselineRenderer( ctxt, 'renderer', self,
                                      loudspeakerConfig=loudspeakerConfig,
                                      numberOfInputs=numberOfInputs,
                                      numberOfOutputs=rendererOutputs, 
                                      interpolationPeriod=interpolationPeriod, 
                                      diffusionFilters=diffusionFilters,
                                      reverbConfig=reverbConfiguration,
                                      sceneReceiverPort=scenePort,
                                      trackingConfiguration=trackingConfiguration 
                                      )
        if not skipConvolver:
            numFilters = brirFilters.numberOfRows
            firLength = brirFilters.numberOfColumns
            numRoutings = brirRouting.size
            self.convolver = rcl.FirFilterMatrix( ctxt, 'convolver', self )
            self.convolver.setup( numberOfInputs=rendererOutputs,
                                 numberOfOutputs=2,
                                 maxFilters=numFilters,
                                 filterLength=firLength,
                                 maxRoutings=numRoutings,
                                 filters=brirFilters,
                                 routings=brirRouting,
                                 controlInputs=False
                                 )
        self.audioIn = visr.AudioInputFloat( "audioIn", self, numberOfInputs )
        if skipConvolver:
            self.audioOut = visr.AudioOutputFloat( "audioOut", self, rendererOutputs )
        else:
            self.audioOut = visr.AudioOutputFloat( "audioOut", self, 2 )
        self.audioConnection( self.audioIn, self.coreRenderer.audioPort("input"))
        if skipConvolver:
            self.audioConnection( self.coreRenderer.audioPort("output"),
                                 self.audioOut )
        else:
            self.audioConnection( self.coreRenderer.audioPort("output"),
                                 self.convolver.audioPort("in"))
            self.audioConnection( self.convolver.audioPort("out"), self.audioOut )
        if len(trackingConfiguration) > 0:
            self.posIn = visr.ParameterInput( "posIn", self,
                                             pml.ListenerPosition.staticType,
                                             pml.DoubleBufferingProtocol.staticType,
                                             pml.EmptyParameterConfig() )
            self.parameterConnection( self.posIn, self.coreRenderer.parameterPort("trackingPositionInput") )



# Get VISR base directory from rsao subdirectory.
visrBaseDirectory = os.path.normpath(os.path.join( os.getcwd(), '../../../..' )).replace('\\','/')

blockSize = 1024
samplingFrequency = 48000
parameterUpdatePeriod = 1024

numBlocks = 128
signalLength = blockSize * numBlocks
t = 1.0/samplingFrequency * np.arange(0,signalLength)

numObjects = 5;

ctxt = visr.SignalFlowContext( blockSize, samplingFrequency)

lspConfigFile  = os.path.join( visrBaseDirectory, 'config/bbc/bs2051-4+5+0.xml')
# lspConfigFile  = os.path.join( visrBaseDirectory, 'config/isvr/audiolab_39speakers_1subwoofer.xml' )

lc = panning.LoudspeakerArray( lspConfigFile )

numOutputChannels = np.max( lc.channelIndices() + lc.subwooferChannelIndices() ) +1
numLoudspeakers = lc.numberOfRegularLoudspeakers

diffFilterFile = os.path.join( visrBaseDirectory, 'config/filters/random_phase_allpass_64ch_512taps.wav')
diffFiltersRaw = np.array(pml.MatrixParameterFloat.fromAudioFile( diffFilterFile ),
                          dtype = np.float32 )
diffFilters = pml.MatrixParameterFloat( diffFiltersRaw[ np.array(lc.channelIndices() )-1,: ] )

reverbConfigStr = '{ "numReverbObjects": %i, "discreteReflectionsPerObject": 20, "lateReverbFilterLength": 2.0, "lateReverbDecorrelationFilters": "%s/config/filters/random_phase_allpass_64ch_1024taps.wav" }' % (numObjects, visrBaseDirectory )

## Load the BBC BRIR dataset
brirFile = os.path.join( os.getcwd(), 'BBC_BRIR.mat' )
brirMat =  h5py.File( brirFile )
brirFull = np.array( brirMat['h_sweetspot'], dtype=np.float32 ).copy('C')
# Scalefactor to compensate for the very low amplitudes of the BBC BRIRs
brirScaleFactor = 500;
brirFlat = brirScaleFactor * np.concatenate( (brirFull[:,0,:], brirFull[:,1,:] ) ) 
brirFilterParam = pml.MatrixParameterFloat( brirFlat, 16 )
numBrirSpeakers = brirFull.shape[0]
# Define the routing for the binaural convolver such that it matches the organisation of the
# flat BRIR matrix.
filterRouting = pml.FilterRoutingList()
for idx in range(0, numBrirSpeakers ):
    filterRouting.addRouting( idx, 0, idx, 1.0 )
    filterRouting.addRouting( idx, 1, idx+numBrirSpeakers, 1.0 )


renderer = ReverbToBinaural( ctxt, 'top', None,
                          loudspeakerConfig=lc,
                          numberOfInputs=2,
                          rendererOutputs=numBrirSpeakers, 
                          interpolationPeriod=parameterUpdatePeriod, 
                          diffusionFilters=diffFilters,
                          trackingConfiguration='',
                          brirFilters = brirFilterParam,
                          brirRouting = filterRouting,
                          reverbConfiguration=reverbConfigStr,
                          scenePort = 4242
                          )

print( 'Created renderer.' )

flow = rrl.AudioSignalFlow( renderer )

## Create the object vector
#jsonFile = os.path.join( visrBaseDirectory, 'src/libobjectmodel/test/data/point_source_with_reverb_1.json' )
#obj_vector=om.ObjectVector() # load the object vector
#obj_str=open(jsonFile).read() # read json object vector as a string
#obj_vector.fillFromJson(obj_str) # populate the object vector
#ro = obj_vector[0]
#                       
#paramInput = flow.parameterReceivePort('objectDataInput')

inputSignal = np.zeros( (1, signalLength ), dtype=np.float32 )
# inputSignal[0,:] = 0.75*np.sin( 2.0*np.pi*440 * t )
inputSignal[ 0, 100 ] = 1

outputSignal = np.zeros( (numOutputChannels, signalLength ), dtype=np.float32 )

for blockIdx in range(0,numBlocks):
#    if blockIdx % (parameterUpdatePeriod/blockSize) == 0:
#        ov = paramInput.data()
#        ov.clear()
#        ov.set(  ro.objectId, ro )
#        paramInput.swapBuffers()
        
    inputBlock = inputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize]
    outputBlock = flow.process( inputBlock )
    outputSignal[:, blockIdx*blockSize:(blockIdx+1)*blockSize] = outputBlock


plt.figure(1)
plt.plot( t, outputSignal[2,:], 'bo-', t, outputSignal[6,:], 'rx-',  t, outputSignal[21,:], 'm.-' )
plt.show( block = False )
