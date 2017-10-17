#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Sep 14 14:55:25 2017

@author: gc1y17
"""
def sph2cart(az,el,r):
    x = r*np.cos(az)*np.cos(el)
    y = r*np.sin(az)*np.cos(el)
    z = r*np.sin(el)
    return x,y,z

from dynamic_binaural_renderer_serial import DynamicBinauralRendererSerial
import time
import visr
import rrl
from rotationFunctions import deg2rad
import objectmodel
import numpy as np
import audiointerfaces as ai
import os
from urllib.request import urlretrieve
from extractDelayInSofaFile import extractDelayInSofaFile

fs = 48000
blockSize = 1024
numBinauralObjects = 1
numOutputChannels = 2
port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600

context = visr.SignalFlowContext(blockSize, fs )
enableTracking = True
enableInterpolation = True


dynamicITD = True
sofaFile = './data/dtf b_nh169.sofa'

if not os.path.exists( sofaFile ):
    urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',
                       sofaFile )

if dynamicITD:
    sofaFileTD = './data/dtf b_nh169_timedelay.sofa'
    if not os.path.exists( sofaFileTD ):
        extractDelayInSofaFile( sofaFile, sofaFileTD )
    sofaFile = sofaFileTD        

controller = DynamicBinauralRendererSerial( context, "Controller", None, 
                                           numBinauralObjects, 
                                           port, 
                                           baud, 
                                           sofaFile,
                                           enableSerial = enableTracking, 
                                           dynITD = dynamicITD,
                                           dynILD = False,
                                           hrirInterp = enableInterpolation)
#to be completed

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( controller )

paramInput = flow.parameterReceivePort('objectVector')

az = 0
el = 30
r = 1
x,y,z = sph2cart( deg2rad(az), deg2rad(el), r )
ps1 = objectmodel.PointSource(0)
ps1.x = x
ps1.y = y
ps1.z = z
ps1.level = 0.5
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