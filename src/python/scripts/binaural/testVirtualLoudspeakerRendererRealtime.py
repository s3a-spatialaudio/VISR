#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 31 16:35:10 2017

@author: gc1y17
"""

import audiointerfaces as ai

import time
from extractDelayInSofaFile import extractDelayInSofaFile
from urllib.request import urlretrieve
import os
from virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer
from virtual_loudspeaker_renderer_serial import VirtualLoudspeakerRendererSerial
import visr
import rrl

import numpy as np
import matplotlib.pyplot as plt
      
############ CONFIG ###############        
fs = 48000
blockSize = 512
numLoudspeakers = 12
numOutputChannels = 2;
parameterUpdatePeriod = 1
numBlocks = 72;
BRIRtruncationLength = 2048

useSourceAutoMovement = False
useTracking = True
useDynamicITD = False
useDynamicILD = False
useHRIRinterpolation = True
useSerialPort = True

port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600

###################################

idMatrix = np.identity(3)
signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

sofaFile = './data/SBSBRIR_x0y0.sofa'
if not os.path.exists( sofaFile ):
    urlretrieve( 'http://data.bbcarp.org.uk/sbsbrir/sofa/SBSBRIR_x0y0.sofa',sofaFile )

if useDynamicITD:
    sofaFileTD = './data/SBSBRIR_x0y0_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        extractDelayInSofaFile( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD


context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)
controller = VirtualLoudspeakerRendererSerial( context, "VirtualLoudspeakerRenderer", None, 
                                  numLoudspeakers, 
                                  port, 
                                  baud, 
                                  sofaFile,
                                  enableSerial = useTracking,
                                  dynITD = useDynamicITD,
                                  hrirInterp = useHRIRinterpolation,
                                  irTruncationLength = BRIRtruncationLength
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
jackCfg = """{ "clientname": "VirtualLdRendererSerial",
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