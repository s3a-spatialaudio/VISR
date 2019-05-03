# -*- coding: utf-8 -*-
"""
Created on Sat Feb 24 07:25:02 2018

@author: andi
"""

import visr
import rcl

import numpy as np

ctxt = visr.SignalFlowContext( 64, 48000 )

numInputs = 3
numOutputs = 7

gains = np.array( np.random.rand( numOutputs, numInputs ), dtype=np.float32 )

mtx = rcl.GainMatrix( ctxt, "Matrix", None, numberOfInputs=numInputs, numberOfOutputs=numOutputs,
                     interpolationSteps=0, initialGains=gains )

dv = rcl.DelayVector( ctxt, "DL", None, numberOfChannels=2, maxDelay=1.0, initialDelay = [0.0, 0.1], initialGain=[1.0,0.5])

iDelay = np.asarray([0.0, 0.1], dtype=np.float64)
iGain=np.asarray([1.0,0.5])

dv = rcl.DelayVector( ctxt, "DL", None, numberOfChannels=2, maxDelay=1.0, initialDelay = iDelay, initialGain=iGain )
