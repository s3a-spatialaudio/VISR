#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct 18 12:30:37 2017

@author: gc1y17
"""
from hoa_binaural_renderer_serial import HoaBinauralRendererSerial
import visr
import rcl
import time
import rrl
import numpy as np
import audiointerfaces as ai
import os
from urllib.request import urlretrieve
from extractDelayInSofaFile import extractDelayInSofaFile

class HoaBinauralRendererSAW( visr.CompositeComponent ):
         def __init__( self,
                     context, name, parent, 
                     numberOfObjects,
                     port,
                     baud,
                     maxHoaOrder,
                     sofaFile,
                     interpolationSteps,
                     headTracking,
                     udpReceivePort=4242,
                     headTrackingCalibrationPort = None
                     ):
            super( HoaBinauralRendererSAW, self ).__init__( context, name, parent )
            self.hoaBinauralRenderer = HoaBinauralRendererSerial( context, "HoaBinauralRendererSerial", self,
                                                                numberOfObjects,
                                                                port,
                                                                baud,
                                                                maxHoaOrder,
                                                                sofaFile,
                                                                interpolationSteps,
                                                                headTracking,
                                                                headTrackingCalibrationPort=headTrackingCalibrationPort
                                                                )
            self.sceneReceiver = rcl.UdpReceiver( context, "SceneReceiver", self, 
                                             port=udpReceivePort, 
                                             mode=rcl.UdpReceiver.Mode.Asynchronous )
            self.sceneDecoder = rcl.SceneDecoder( context, "SceneDecoder", self )
            self.parameterConnection( self.sceneReceiver.parameterPort("messageOutput"),
                                 self.sceneDecoder.parameterPort("datagramInput") )
            self.parameterConnection( self.sceneDecoder.parameterPort( "objectVectorOutput"), 
                                 self.hoaBinauralRenderer.parameterPort("objectVector"))

            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
           
            self.audioConnection(  self.objectSignalInput, self.hoaBinauralRenderer.audioPort("audioIn"))
            self.audioConnection( self.hoaBinauralRenderer.audioPort("audioOut"), self.binauralOutput)

            if headTrackingCalibrationPort is not None:
                self.calibrationTriggerReceiver = rcl.UdpReceiver( context, "CalibrationTriggerReceiver", self, port = headTrackingCalibrationPort )
                self.parameterConnection( self.calibrationTriggerReceiver.parameterPort("messageOutput"),
                                         self.hoaBinauralRenderer.parameterPort("headTrackingCalibration"))




############ CONFIG ###############  
fs = 48000
blockSize = 2048
numBinauralObjects = 32
numOutputChannels = 2

# datasets are provided for odd orders 1,3,5,7,9
maxHoaOrder = 3


# switch dynamic tracking on and off.
useTracking = True
if useTracking:
    headTrackingCalibrationPort=8889
else:
    headTrackingCalibrationPort=None

port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600
###################################


context = visr.SignalFlowContext(blockSize, fs )

currDir = os.getcwd()
sofaFile = os.path.join( currDir, './data/bbc_hoa2bin_sofa/Gauss_O%d_ku100_dualband_energy.sofa' % maxHoaOrder )

controller = HoaBinauralRendererSAW( context, "HoaBinauralRendererSerialSAW", None, 
                                            numBinauralObjects, 
                                            port, 
                                            baud, 
                                            maxHoaOrder,
                                            sofaFile,
                                            interpolationSteps = blockSize,
                                            headTracking = useTracking,
                                            headTrackingCalibrationPort = headTrackingCalibrationPort)
#to be completed

result,messages = rrl.checkConnectionIntegrity(controller)
if not result:
   print(messages)

flow = rrl.AudioSignalFlow( controller )
                        
aiConfig = ai.AudioInterface.Configuration( flow.numberOfCaptureChannels,
                                           flow.numberOfPlaybackChannels,
                                           fs,
                                           blockSize )

jackCfg = """{ "clientname": "HOABinRendererSAW",
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