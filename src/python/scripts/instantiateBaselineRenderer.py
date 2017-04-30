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
import pml
import rcl
import rrl

ctxt = visr.SignalFlowContext( 1024, 48000 )

lc = pml.LoudspeakerArray( '/home/andi/dev/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )
#lc = pml.LoudspeakerArray( 'c:/local/visr/config/isvr/audiolab_39speakers_1subwoofer.xml' )

diffFilters = pml.BasicMatrixFloat( 39, 512, 16 )

renderer1 = signalflows.BaselineRenderer( ctxt, 'renderer1', None, lc, 2, 41, 4096, diffFilters, '', 4242, '', False )

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

