# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

import sys
#sys.path.append( '/home/andi/dev/visr-build-debug/python' )
sys.path.append( 'c:/local/visr-build/python/Debug' )


import visr
import signalflows
import panning
import pml
import rcl
import rrl

ctxt = visr.SignalFlowContext( 1024, 48000 )

lc = panning.LoudspeakerArray( '/home/andi/dev/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )
#lc = panning.LoudspeakerArray( 'c:/local/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )

diffFilters = pml.MatrixParameterFloat( 39, 512, 16 )

renderer1 = signalflows.BaselineRenderer( context=ctxt,
                                         name='renderer1',
                                         parent=None,
                                         loudspeakerConfig=lc, 
                                         numberOfInputs=2,
                                         numberOfOutputs=41, 
                                         interpolationPeriod=4096, 
                                         diffusionFilters=diffFilters, 
                                         trackingConfiguration='', 
                                         sceneReceiverPort=4242,
                                         reverbConfig='',
                                         frequencyDependentPanning=False )

print( 'Created renderer.' )

flow1 = rrl.AudioSignalFlow( renderer1 )

cc1 = visr.CompositeComponent( ctxt, 'composite1', None )

numComponents = cc1.numberOfComponents

print( 'Composite has %d components' % ( numComponents) )

add1 = rcl.Add( ctxt, 'adder1', cc1 )
add1.setup( width = 8, numInputs = 2 )

add2 = rcl.Add( ctxt, 'adder2', cc1 )
add2.setup( width = 8, numInputs = 2 )

# In C++, this is a protected method (can be done only from within.)
#r1 = range(0,8)
#l1 = [0,1,2,3,4,5,6,7]
cc1.registerAudioConnection( "adder1", "out", visr.ChannelList( visr.ChannelRange(0,8)), "adder2", "in0", visr.ChannelList( visr.ChannelRange(0,8)) )

numComponents2 = cc1.numberOfComponents

print( 'Composite has now %d components' % ( numComponents2) )

## TODO: Add checking methods to composite components.
#
## Do not attempt this unless the 
#flow2 = rrl.AudioSignalFlow( myFirstComposite )

