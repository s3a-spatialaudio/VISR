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
import objectmodel
import numpy as np
import audiointerfaces as ai

fs = 44100
blockSize = 1024
numBinauralObjects = 60
numOutputChannels = 2
port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600

context = visr.SignalFlowContext(blockSize, fs )
enableTracking = True
enableInterpolation = True
controller = DynamicBinauralRendererSerial( context, "Controller", None, numBinauralObjects, port, baud, 
                                           enableSerial = enableTracking, 
                                           dynamicITD = True,
                                           dynamicILD = False,
                                           hrirInterpolation = enableInterpolation)
#to be completed

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( controller )

paramInput = flow.parameterReceivePort('objectDataInput')

az = 0
el = 0
r = 1
x,y,z = sph2cart( az, el, r )
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