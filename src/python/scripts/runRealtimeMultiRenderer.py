#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Dec  1 10:38:25 2017

@author: andi
"""

from realtime_multi_renderer import RealTimeMultiRenderer
from realtime_multi_renderer_trajectory import  RealTimeMultiRendererTrajectory

import visr
#import panning
import rrl
import audiointerfaces as ai

import os.path
import sys
import time
import numpy as np

fs = 48000
blockSize = 1024

context = visr.SignalFlowContext( blockSize, fs )


# configBasePath = '/usr/share/visr/config/'
configBasePath = '/home/andi/dev/visr/config/'

renderConfigNames = [ 'bbc-listening-room-full.xml',
 'bs2051-0+2+0.xml',
 'bs2051-0+5+0.xml',
 'bs2051-2+5+0.xml',
 'bs2051-4+5+0.xml',
 'bs2051-4+9+0.xml',
 'bs2051-9+10+3.xml' ]

numberOfOutputs = 34 # Maximum channel number

# Create filtes with full paths
configPaths = [ os.path.join( configBasePath, 'bbc', cfg ) for cfg in renderConfigNames ]

diffusionFilter = os.path.join( configBasePath, 'filters/random_phase_allpass_64ch_512taps.wav' )


trajectoryGenerator = True

if trajectoryGenerator:
    updatePeriod = 4096
    cycleDuration = 5.0

    numPos = int(np.ceil( cycleDuration*fs / updatePeriod ))

    # Example source trajectory (admittedly a stupid one)
    azimuths = np.arange( 0, 2.0*np.pi, 2.0*np.pi/numPos )
    elevations = 0.25 * np.sin(azimuths)
    radius = 1.0
    # Could be replaced by a sph2cart library function.
    positions = radius * np.stack( (np.cos(azimuths)*np.cos(elevations),
                         np.sin(azimuths)*np.cos(elevations),
                          np.sin(elevations))
                         )
    renderer = RealTimeMultiRendererTrajectory( context, "MultiRenderer", None,
                     loudspeakerConfigFiles=configPaths,
                     numberOfInputs=4,
                     numberOfOutputs=34,
                     interpolationPeriod=blockSize,
                     diffusionFilterFile = diffusionFilter,
                     trajectoryPositions = positions,
                     trajectoryUpdateSamples = updatePeriod,
                     controlReceivePort=4245 )
else:
    renderer = RealTimeMultiRenderer( context, "MultiRenderer", None,
                     loudspeakerConfigFiles=configPaths,
                     numberOfInputs=4,
                     numberOfOutputs=34,
                     interpolationPeriod=blockSize,
                     diffusionFilterFile = diffusionFilter,
                     udpReceivePort=4242,
                     controlReceivePort=4245 )

flowCheck, messages = rrl.checkConnectionIntegrity( renderer )
if not flowCheck:
    print( "Integrity check for multirenderer failed: %s" % messages )
    # sys.exit()

flow = rrl.AudioSignalFlow( renderer )

aiConfig = ai.AudioInterface.Configuration( flow.numberOfCaptureChannels,
                                           flow.numberOfPlaybackChannels,
                                           fs,
                                           blockSize )

jackCfg = """{ "clientname": "MultiRenderer",
  "autoconnect" : "false",
  "portconfig":
  {
    "capture":  [{ "basename":"inObj_", "externalport" : {} }],
    "playback": [{ "basename":"outBin_", "externalport" : {} }]
  }
}"""

aIfc = ai.AudioInterfaceFactory.create("Jack", aiConfig, jackCfg)

aIfc.registerCallback( flow )

aIfc.start()

print( "Rendering started." )
#
time.sleep(1)
i = input("Enter text (or Enter to quit): ")
if not i:
    aIfc.stop()
    aIfc.unregisterCallback()
    del aIfc


