#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 14 14:55:25 2017

@author: gc1y17
"""

from dynamic_binaural_renderer_serial import DynamicBinauralRendererSerial
import time
import visr
import rrl
from rotationFunctions import deg2rad, sph2cart3inp
import objectmodel
import audiointerfaces as ai
from extractDelayInSofaFile import extractDelayInSofaFile

import os
from urllib.request import urlretrieve
from sys import platform

############ CONFIG ###############
fs = 48000
blockSize = 1024
numBinauralObjects = 64
numOutputChannels = 2;

# switch dynamic tracking on and off.
useTracking = True
useDynamicITD = False
useDynamicILD = False
useHRIRinterpolation = True
useCrossfading = True


# TODO: Check and adkust port names for the individual system
if platform == 'linux' or platform == 'linux2':
    port = "/dev/ttyUSB0"
elif platform == 'darwin':
    port = "/dev/cu.usbserial-AJ03GR8O"
elif platform == 'windows':
    port = "COM10"

baud = 57600
###################################


context = visr.SignalFlowContext(blockSize, fs )

sofaFile = './data/dtf b_nh169.sofa'

if not os.path.exists( sofaFile ):
    urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',
                       sofaFile )

if useDynamicITD:
    sofaFileTD = './data/dtf b_nh169_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        extractDelayInSofaFile( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD

renderer = DynamicBinauralRendererSerial( context, "Controller", None,
                                         numBinauralObjects,
                                         port,
                                         baud,
                                         sofaFile,
                                         enableSerial = useTracking,
                                         dynITD = useDynamicITD,
                                         dynILD = useDynamicITD,
                                         hrirInterp = useHRIRinterpolation,
                                         filterCrossfading=useCrossfading
                                         )
#to be completed

result,messages = rrl.checkConnectionIntegrity(renderer)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( renderer )

paramInput = flow.parameterReceivePort('objectVector')

az = 0
el = 0
r = 1
x,y,z = sph2cart3inp( deg2rad(az), deg2rad(el), r )
ps1 = objectmodel.PointSource(0)
ps1.x = x
ps1.y = y
ps1.z = z
#same level as HRIR renderer
#ps1.level = 0.8

#same level as HOA renderer
ps1.level = 5
ps1.groupId = 5
ps1.priority = 5
ps1.resetNumberOfChannels(1)
ps1.setChannelIndex(0,ps1.objectId)

ov = paramInput.data()
ov.clear()
ov.insert( ps1 )
paramInput.swapBuffers()

aiConfig = ai.AudioInterface.Configuration( flow.numberOfCaptureChannels,
                                           flow.numberOfPlaybackChannels,
                                           fs,
                                           blockSize )

jackCfg = """{ "clientname": "BinRendererSerial",
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
