# -*- coding: utf-8 -*-

from visr_bst import RealtimeVirtualLoudspeakerRenderer
from visr_bst.tracker import RazorAHRSWithUdpCalibrationTrigger

import visr
import rrl
import audiointerfaces as ai

import os
#import time
from sys import platform
from urllib.request import urlretrieve # Load SOFA files on the fly


# %% ############ CONFIG ###############
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

# %% Configure the tracking device (if used)
if useTracking:
    # Specific settings for the Razor AHRS
    # TODO: Check and adjust port names for the individual system
    if platform == 'linux' or platform == 'linux2':
        port = "/dev/ttyUSB0"
    elif platform == 'darwin':
        port = "/dev/cu.usbserial-AJ03GSC8"
    elif platform in ['windows','win32']:
        port = "COM4"

    headTrackingCalibrationPort = 8889

    # Python class of the tracking receiver
    headTrackingDevice = RazorAHRSWithUdpCalibrationTrigger

    headTrackingPositionalArguments = None # Use only keyword arguments
    headTrackingKeywordArguments = {'port': port, 'calibrationPort': headTrackingCalibrationPort }

    headTrackingCalibrationPort=8889
else:
    headTrackingCalibrationPort=None

# %% Configure the audio interface
if platform in ['linux', 'linux2', 'darwin' ]:
    # Either use PortAudio
#    audioIfcName = "PortAudio"
#    audioIfcCfg = """{ "hostapi": "CoreAudio" }""" # Mac OS X
#    audioIfcCfg = """{ "hostapi": "ALSA" }"""      # Linux

    # Or use Jack on Linux or Mac OS X
    audioIfcCfg = """{ "clientname": "BstRenderer",
      "autoconnect" : "false",
      "portconfig":
      {
        "capture":  [{ "basename":"in", "externalport" : {} }],
        "playback": [{ "basename":"out", "externalport" : {} }]
      }
    }"""
    audioIfcName = "Jack"
elif platform in ['windows', 'win32' ]:
    audioIfcCfg = """{ "hostapi": "WASAPI" }"""
#    audioIfcCfg = """{ "hostapi": "ASIO" }"""   # If you have a professional audio interface with an ASIO driver
    audioIfcName = "PortAudio"

###################################

sofaDirectory = './data'

sofaFile = 'bbcrdlr_systemA.sofa' # BS-2051 0+2+0 (Stereo)
#sofaFile = 'bbcrdlr_systemB.sofa' # BS-2051 0+5+0 (5.1)
#sofaFile = 'bbcrdlr_systemD.sofa' # BS-2051 4+5+0

fullSofaPath = os.path.join( sofaDirectory, sofaFile )

if not os.path.exists( fullSofaPath ):
    if not os.path.exists( sofaDirectory ):
        os.mkdir( sofaDirectory )
    urlretrieve( 'http://data.bbcarp.org.uk/bbcrd-brirs/sofa/' + sofaFile,
                fullSofaPath )


context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

renderer = RealtimeVirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", None,
                                  sofaFile = fullSofaPath,
                                  dynamicITD = useDynamicITD,
                                  hrirInterpolation = useHRIRinterpolation,
                                  irTruncationLength = BRIRtruncationLength,
                                  filterCrossfading = useCrossfading,
                                  interpolatingConvolver=useInterpolatingConvolver,
                                  headTrackingReceiver = headTrackingDevice,
                                  headTrackingPositionalArguments = headTrackingPositionalArguments,
                                  headTrackingKeywordArguments = headTrackingKeywordArguments,
                                  fftImplementation = fftImplementation
                                  )

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)


flow = rrl.AudioSignalFlow( renderer )

aiConfig = ai.AudioInterface.Configuration( flow.numberOfCaptureChannels,
                                           flow.numberOfPlaybackChannels,
                                           fs,
                                           blockSize )

aIfc = ai.AudioInterfaceFactory.create(audioIfcName, aiConfig, audioIfcCfg)

aIfc.registerCallback( flow )

aIfc.start()

print( "Rendering started. Press <q><Return> to quit." )
while( True ):
    i = input( "Press <q><Return> to quit." )
    if i in ['q','Q']:
        break

aIfc.stop()
aIfc.unregisterCallback()
del aIfc
del flow
del renderer