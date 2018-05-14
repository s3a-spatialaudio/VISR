# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import rrl
import audiointerfaces as ai

from visr_bst import RealtimeDynamicHrirRenderer
from visr_bst.tracker import RazorAHRSWithUdpCalibrationTrigger

from visr_bst.util import sofaExtractDelay

import os
from urllib.request import urlretrieve
from sys import platform

############ CONFIG ###############
fs = 48000
blockSize = 1024
numBinauralObjects = 2

useTracking = True      # switch dynamic tracking on and off.
useDynamicITD = False
useDynamicILD = False
useHRIRinterpolation = True
useCrossfading = True
fftImplementation = 'ffts'

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

sofaFile = './data/dtf b_nh169.sofa'
if not os.path.exists( sofaFile ):
    if not os.path.exists( './data/' ):
        os.mkdir( './data/' )
    urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',sofaFile )

# Extract the time delay from the SOFA data and store it in the 'Data.Delay' field.
if useDynamicITD:
    sofaFileTD = './data/dtf b_nh169_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        sofaExtractDelay( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD

# %% Create and initialise the signal flow

context = visr.SignalFlowContext(blockSize, fs )

renderer = RealtimeDynamicHrirRenderer( context, "HrirRenderer", None,
                                       numberOfObjects = numBinauralObjects,
                                       sofaFile = sofaFile,
                                       dynamicITD = useDynamicITD,
                                       dynamicILD = useDynamicILD,
                                       hrirInterpolation = useHRIRinterpolation,
                                       filterCrossfading=useCrossfading,
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