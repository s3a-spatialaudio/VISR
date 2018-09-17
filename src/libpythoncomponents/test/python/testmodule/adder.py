# -*- coding: utf-8 -*-

# Copyright (C) 2017-2018 Andreas Franck
# Copyright (C) 2017-2018 University of Southampton

"""
Simple atomic component implmented in Python to be used in a unit test.
"""

import visr

import numpy as np

class Adder( visr.AtomicComponent ):
    """ General-purpose add block for an arbitrary number of inputs"""
    def __init__( self, context, name, parent, numInputs, width ):
        super( Adder, self ).__init__( context, name, parent )
        self.output = visr.AudioOutputFloat( "out", self, width )
        self.inputs = []
        for inputIdx in range( 0, numInputs ):
            portName =  "in%d" % inputIdx
            inPort = visr.AudioInputFloat( portName, self, width )
            self.inputs.append( inPort )
    def process( self ):
        if len( self.inputs ) == 0:
            self.output.set( np.zeros( (self.output.width, self.period ), dtype = np.float32 ) )
        elif len( self.inputs ) == 1:
            self.output.set( np.array(self.inputs[0]))
        else:
            acc = np.array(self.inputs[0])
            for idx in range(1,len(self.inputs)):
                acc += np.array(self.inputs[idx])
            self.output.set( acc )
