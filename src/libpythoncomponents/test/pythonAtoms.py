# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import visr

import numpy as np


class PythonAdder3( visr.AtomicComponent ):
    """ Simple adder for three inputs """
    def __init__( self, context, name, parent, width ):
        super(PythonAdder3,self).__init__( context, name, parent )
        self.input0 = visr.AudioInputFloat( "in0", self, width )
        self.input1 = visr.AudioInputFloat( "in1", self, width )
        self.input2 = visr.AudioInputFloat( "in2", self, width )
        self.output = visr.AudioOutputFloat( "out", self, width )
    def process( self ):
        self.status( visr.StatusMessage.Information, "Called PythonAdder3::process()" )
        self.output.set( self.input0.data() + self.input1.data() + self.input2.data() )


class PythonAdder( visr.AtomicComponent ):
    """ General-purpose add block for an arbitrary number of inputs"""
    def __init__( self, context, name, parent, numInputs, width ):
        super(PythonAdder,self).__init__( context, name, parent )
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
