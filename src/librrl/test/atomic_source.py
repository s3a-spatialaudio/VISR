# -*- coding: utf-8 -*-
"""
Created on Fri Feb  2 18:55:39 2018

@author: af5u13
"""

import visr
import rrl

import numpy as np

class AtomicSource( visr.AtomicComponent):
    def __init__( self, context, name, parent = None ):
        super().__init__( context, name, parent )
        self.audioOut = visr.AudioOutputFloat( "out", self, width = 1 )

    def process(self):
        n = self.period()
        self.audioOut.set( np.zeros( (1,n), dtype = np.float32 ) )


class CompositeSource( visr.CompositeComponent ):
    def __init__( self, context, name, parent = None ):
        super().__init__( context, name, parent )
        self.audioOut = visr.AudioOutputFloat( "out", self, width = 1 )
        self.atom = AtomicSource( context, "atom", self )
        self.audioConnection( self.atom.audioPort("out"), self.audioOut )


fs = 48000
bs = 1024
context = visr.SignalFlowContext( period=bs, samplingFrequency=fs )

comp = CompositeSource( context, "top", None )
# comp = AtomicSource( context, "top", None )

flow = rrl.AudioSignalFlow( comp )