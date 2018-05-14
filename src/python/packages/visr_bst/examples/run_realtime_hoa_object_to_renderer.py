# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import rrl
import audiointerfaces as ai

from visr_bst import RealtimeHoaObjectToBinauralRenderer
from visr_bst.tracker import RazorAHRSWithUdpCalibrationTrigger

import os
from sys import platform, exit

############ CONFIG ###############
fs = 48000
blockSize = 1024
numBinauralObjects = 2

useTracking = True      # switch dynamic tracking on and off.
fftImplementation = 'ffts'
hoaOrder = 5 # HOA order for object encoding and sound field decoding
sceneReceivePort = 4242 # UDP port to receive scene metadata, use 'None' to disable

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


# TODO: Replace path
sofaFile = 'c:/local/SOFA/bbc_hoa2bin/Gauss_O%d_ku100_dualband_energy.sofa' % hoaOrder

if not os.path.exists( sofaFile ):
    print( "SOFA file containing HOA decoding filters not found")
    exit( 1 )

# %% Create and initialise the signal flow

context = visr.SignalFlowContext(blockSize, fs )

renderer = RealtimeHoaObjectToBinauralRenderer( context, "HoaRenderer", None,
                                       numberOfObjects = numBinauralObjects,
                                       maxHoaOrder = hoaOrder,
                                       sofaFile = sofaFile,
                                       interpolationSteps = context.period,
                                       headTrackingReceiver = headTrackingDevice,
                                       headTrackingPositionalArguments = headTrackingPositionalArguments,
                                       headTrackingKeywordArguments = headTrackingKeywordArguments,
                                       sceneReceiveUdpPort = sceneReceivePort,
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