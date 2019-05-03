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

from metadapter import Component as Metadapter

from metadapter.thirdparty import OSC

context = visr.SignalFlowContext( 64, 48000 )

objectVectorOutput = False

oscMessages = True


mda = Metadapter( context, "Metadapter", None, 'c:/local/dev/metadapter/config/radio_drama_adaptation_demo.xml',
                 objectVectorInput = True,
                 objectVectorOutput = objectVectorOutput,
                 oscControlPort = oscMessages )

flow = rrl.AudioSignalFlow( mda )

objInput = flow.parameterReceivePort( "objectIn" )

if oscMessages:
    oscInput = flow.parameterReceivePort( "oscControlIn" )

objOutput = flow.parameterSendPort( "objectOut" )

ps = om.PointSource( 0 )
ps.level = 0.375
ps.position = [ -2, -3, 0.5 ]

if oscMessages:
    msg = OSC.OSCMessage( "/groupVolume/narrator" )
    msg.append( float(-6.0) )
    binMsg = msg.getBinary()
    msgParam = pml.StringParameter(binMsg)
    oscInput.enqueue( msgParam )

objInput.data().set( [ps] )
objInput.swapBuffers()

flow.process()

if objectVectorOutput:
    if objOutput.changed():
        ovOut = objOutput.data()
        objOutput.resetChanged()
else:
    while not objOutput.empty():
        print( objOutput.front() )
        objOutput.pop()