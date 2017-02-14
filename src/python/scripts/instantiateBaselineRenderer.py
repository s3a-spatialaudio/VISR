# -*- coding: utf-8 -*-
"""
Created on Tue Feb 14 15:59:11 2017

@author: af5u13
"""

import sys
sys.path.append( 'c://local/visr-build/python/Debug/' )

import visr
import signalflows
import pml

ctxt = visr.SignalFlowContext( 1024, 48000 )

lc = pml.LoudspeakerArray( 'c:\\local\\visr\\config\\isvr\\audiolab_39speakers_1subwoofer.xml' )

diffFilters = pml.MatrixParameterFloat( 39, 512, 16 )

renderer1 = br = signalflows.BaselineRenderer( ctxt, 'renderer1', None, lc, 2, 41, 4096, diffFilters, '', 4242, '', False )

print( 'Renderer type: %s' % type( renderer1 ) )