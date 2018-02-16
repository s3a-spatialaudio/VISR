#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 31 16:35:10 2017

@author: gc1y17
"""

import audiointerfaces as ai



from virtual_loudspeaker_renderer_serial import VirtualLoudspeakerRendererSerial
import visr
import rrl

import time
from sys import platform

############ CONFIG ###############
fs = 48000
blockSize = 1024
numOutputChannels = 2;
parameterUpdatePeriod = 1
numBlocks = 72;
BRIRtruncationLength = None

useTracking = True
useDynamicITD = False
useHRIRinterpolation = True
useSerialPort = True
useCrossfading = True
useInterpolatingConvolver = True
fftImplementation = 'ffts'

# TODO: Check and adjust port names for the individual system
if platform == 'linux' or platform == 'linux2':
    port = "/dev/ttyUSB0"
elif platform == 'darwin':
    port = "/dev/cu.usbserial-AJ03GSC8"
elif platform == 'windows':
    port = "COM10"
baud = 57600


if useTracking:
    headTrackingCalibrationPort=8889
else:
    headTrackingCalibrationPort=None

###################################

# TODO: Select the path and the SOFA file.
sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemA.sofa' # Stereo
sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemB.sofa' # 5.1

#sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemA.sofa'
## Set the number of loudspeakers accordingly.
#numLoudspeakers = 2

#sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemH.sofa'
## Set the number of loudspeakers accordingly.
#numLoudspeakers = 22
#
#sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemF.sofa'
## Set the number of loudspeakers accordingly.
#numLoudspeakers = 10

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

controller = VirtualLoudspeakerRendererSerial( context, "VirtualLoudspeakerRenderer", None,
                                  port,
                                  baud,
                                  sofaFile,
                                  enableSerial = useTracking,
                                  dynITD = useDynamicITD,
                                  hrirInterp = useHRIRinterpolation,
                                  irTruncationLength = BRIRtruncationLength,
                                  headTrackingCalibrationPort=headTrackingCalibrationPort,
                                  filterCrossfading = useCrossfading,
                                  interpolatingConvolver=useInterpolatingConvolver,
                                  fftImplementation = fftImplementation
                                  )

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)


flow = rrl.AudioSignalFlow( controller )

if not useSerialPort and useTracking:
    trackingInput = flow.parameterReceivePort( "tracking" )

aiConfig = ai.AudioInterface.Configuration( flow.numberOfCaptureChannels,
                                           flow.numberOfPlaybackChannels,
                                           fs,
                                           blockSize )
#if client name is too long you get an error
jackCfg = """{ "clientname": "VirtualLoudspeakerRenderer",
  "autoconnect" : "false",
  "portconfig":
  {
    "capture":  [{ "basename":"in", "externalport" : {} }],
    "playback": [{ "basename":"out", "externalport" : {} }]
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