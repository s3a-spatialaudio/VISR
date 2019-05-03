#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 31 16:35:10 2017

@author: gc1y17
"""

import audiointerfaces as ai

import pml
import rcl
import time
from virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer
from virtual_loudspeaker_renderer_serial import VirtualLoudspeakerRendererSerial
import visr
import rrl

import os
import numpy as np
import matplotlib.pyplot as plt

from sys import platform

class AudioSignalSwitch( visr.AtomicComponent ):
    def __init__( self, context, name, parent,
                 numberOfChannels,
                 numberOfInputs,
                 initialInput=0,
                 controlDataType = pml.UnsignedInteger # Data type
                 ):
        super( AudioSignalSwitch, self ).__init__( context, name, parent )
        self.inputs = []
        for inIdx in range(0, numberOfInputs):
            portName = "in_%d" % inIdx
            self.inputs.append( visr.AudioInputFloat( portName, self, numberOfChannels ) )

        self.output = visr.AudioOutputFloat( "out", self, numberOfChannels )
        self.controlInput = visr.ParameterInput( "controlIn", self,
                                                protocolType=pml.MessageQueueProtocol.staticType,
                                                parameterType=controlDataType.staticType,
                                                parameterConfig=pml.EmptyParameterConfig() )
        self.activeInput = initialInput

    def process( self ):
        protocolIn = self.controlInput.protocolInput()
        if not protocolIn.empty():
            while not protocolIn.empty():
                newInput = int(protocolIn.front().value)
                protocolIn.pop()
            if newInput >= len(self.inputs):
                raise IndexError("Control input exceeds number of ports")
            if self.activeInput != newInput:
                self.activeInput = newInput
                # print( "OutputSwitch: Switched to output #%i." % newInput )
        sig = self.inputs[self.activeInput].data()
        self.output.set( sig )

class Comparison( visr.CompositeComponent ):
    def __init__(self,
                 context, name, parent,
                 numLoudspeakers,
                 port,
                 baud,
                 sofaFile,
                 enableSerial = True,
                 dynITD = True,
                 hrirInterp = True,
                 irTruncationLength = None,
                 headTrackingCalibrationPort = None,
                 switchUdpPort = 12345
                 ):
        super( Comparison, self ).__init__( context, name, parent )
        self.input = visr.AudioInputFloat( "in", self, 2 )
        self.output = visr.AudioOutputFloat( "out", self, 2 )

        self.renderer = VirtualLoudspeakerRendererSerial( context, "renderer", self,
                                                         numLoudspeakers,
                                                         port,
                                                         baud,
                                                         sofaFile,
                                                         enableSerial = useTracking,
                                                         dynITD = useDynamicITD,
                                                         hrirInterp = useHRIRinterpolation,
                                                         irTruncationLength = BRIRtruncationLength,
                                                         headTrackingCalibrationPort=headTrackingCalibrationPort
                                                         )

        self.controlReceiver = rcl.UdpReceiver( context, "ControlReceiver", self,
                                               port=switchUdpPort,
                                               mode=rcl.UdpReceiver.Mode.Asynchronous)
        self.switch = AudioSignalSwitch( context, "OutputSwitch", self,
                                         numberOfChannels=2,
                                         numberOfInputs=2,
                                         controlDataType=pml.Float
                                         )
        self.controlDecoder = rcl.ScalarOscDecoder( context, "ControlDecoder", self )
        self.controlDecoder.setup(dataType='float')
        self.parameterConnection( self.controlReceiver.parameterPort("messageOutput"),
                                 self.controlDecoder.parameterPort("datagramInput") )
        self.parameterConnection( self.controlDecoder.parameterPort( "dataOut"),
                                 self.switch.parameterPort( "controlIn" ) )

        self.audioConnection( self.input, self.renderer.audioPort("audioIn"))
        self.audioConnection( self.renderer.audioPort("audioOut"), self.switch.audioPort("in_0") )
        self.audioConnection( self.input, self.switch.audioPort("in_1"))
        self.audioConnection( self.switch.audioPort("out"), self.output )

############ CONFIG ###############
fs = 48000
blockSize = 1024
numOutputChannels = 2;
parameterUpdatePeriod = 1
numBlocks = 72;
BRIRtruncationLength = 16384

useTracking = True
useDynamicITD = False
useHRIRinterpolation = True
useSerialPort = True

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

idMatrix = np.identity(3)
signalLength = blockSize * numBlocks
t = 1.0/fs * np.arange(0,signalLength)

# TODO: Select the path and the SOFA file.
sofaFile = '/home/andi/BBC/SOFA/bbcrdlr_systemA.sofa'
# Set the number of loudspeakers accordingly.
numLoudspeakers = 2

context = visr.SignalFlowContext( period=blockSize, samplingFrequency=fs)

controller = Comparison( context, "VirtualLoudspeakerRenderer", None,
                                  numLoudspeakers,
                                  port,
                                  baud,
                                  sofaFile,
                                  enableSerial = useTracking,
                                  dynITD = useDynamicITD,
                                  hrirInterp = useHRIRinterpolation,
                                  irTruncationLength = BRIRtruncationLength,
                                  headTrackingCalibrationPort=headTrackingCalibrationPort,
                                  switchUdpPort = 12345
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