# -*- coding: utf-8 -*-
"""
Created on Sun Feb 26 16:41:01 2017

@author: andi
"""

import sys
sys.path.append( '/home/andi/dev/visr-build-debug/python' )

import visr
import pml # This loads the parameter types and protocols used.
import rcl
import rrl

# Usage in debugger:
# exec(open("./transauralCoeffCalculator.py").read())

import numpy as np
import matplotlib.pyplot as plt

class TransauralCoefficientCalculator( visr.AtomicComponent ):
    """Calculator for determining filter coefficients and gain/delay coefficients based on multiple listeners"""
    def __init__( self, context, name, parent, arrayConfiguration, numberOfListeners=2, firLength=1024 ):
        super(TransauralCoefficientCalculator,self).__init__( context, name, parent ) # Call base constructor
        
        totalNumberOfFilters = numberOfListeners * 2 # Correct ???
        
        # Create a parameter input for each listening 
        listPosParamType = pml.ListenerPosition.staticType
        doubleBufferProtocolType = pml.DoubleBufferingProtocol.staticType
        emptyCfg = pml.EmptyParameterConfig() # Need to provide an 'empty' parameter config.
        self.listenerInputs = []
        for listenerIdx in range(1, numberOfListeners ):
            portName = "listener%d" % listenerIdx
            self.listenerInputs.append( visr.ParameterInput(portName, self,
              listPosParamType, doubleBufferProtocolType, emptyCfg ) )
            
    def process( self ):
        self.status( visr.StatusMessage.Information, "Called TransauralCoefficientCalculator::process()" )





fs = 48000

blockSize = 64

numberOfChannels = 2

c = visr.SignalFlowContext(blockSize, fs )

lcx = np.arange(-1,1,0.1,dtype=np.float32)
lcy = np.zeros( lcx.shape )
lc = np.vstack((lcx,lcy) ).transpose()


tcc = TransauralCoefficientCalculator( c, "CoefficientCalculator", None, arrayConfiguration=lc, numberOfListeners=3, firLength=1024 )
