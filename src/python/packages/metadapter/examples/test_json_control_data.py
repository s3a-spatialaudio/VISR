#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar  8 17:36:28 2018

@author: af5u13
"""

import visr
import rrl
import objectmodel as om
import pml

import os

from metadapter import Component as Metadapter

from metadapter.thirdparty import OSC

context = visr.SignalFlowContext( 64, 48000 )

objectVectorOutput = False

oscMessages = False
jsonMessages = True

processorConf = os.path.join( os.getcwd(), '../config/radio_drama_adaptation_demo.xml' )

mda = Metadapter( context, "Metadapter", None, processorConf,
                 objectVectorInput = True,
                 objectVectorOutput = objectVectorOutput,
                 oscControlPort = oscMessages,
                 jsonControlPort = jsonMessages )

flow = rrl.AudioSignalFlow( mda )

objInput = flow.parameterReceivePort( "objectIn" )

if oscMessages:
    oscInput = flow.parameterReceivePort( "oscControlIn" )

if jsonMessages:
    jsonInput = flow.parameterReceivePort( "jsonControlIn" )

objOutput = flow.parameterSendPort( "objectOut" )

ps0 = om.PointSource( 0 )
ps0.level = 1.0
ps0.position = [ -2, -3, 0.5 ]
ps1 = om.PointSource( 1 )
ps1.level = 1.0
ps1.position = [ -4, 1, -0.5 ]


newVolume = 6.02

if oscMessages:
    msg = OSC.OSCMessage( "/groupVolume/narrator" )
    msg.append( float(-6.0) )
    binMsg = msg.getBinary()
    msgParam = pml.StringParameter(binMsg)
    oscInput.enqueue( msgParam )

if jsonMessages:
    msg = """{ "groupVolume": [{ "narrator": [ %f ] }, { "dialogue": [ -16.02 ] }], "GlobalAttenuation": [{"volume": [-20.0] }] }""" % newVolume
    msgParam = pml.StringParameter(msg)
    jsonInput.enqueue( msgParam )

objInput.data().set( [ps0, ps1] )
objInput.swapBuffers()

flow.process()

if objectVectorOutput:
    if objOutput.changed():
        ovOut = objOutput.data()
        objOutput.resetChanged()
else:
    while not objOutput.empty():
        print( objOutput.front().str )
        objOutput.pop()