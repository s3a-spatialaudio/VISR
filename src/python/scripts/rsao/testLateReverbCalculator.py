#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Aug 16 13:44:46 2017

@author: pdc
"""

import visr
import objectmodel as om
import reverbobject as ro
import panning
import rrl
import rcl
import numpy as np
import matplotlib.pyplot as plt
import os.path


# Get VISR base directory from rsao subdirectory.
visrBaseDirectory = os.path.join( os.getcwd(), '../../../..' )

jsonFile = os.path.join( visrBaseDirectory, 'src/libobjectmodel/test/data/point_source_with_reverb_1.json' )

obj_vector=om.ObjectVector() # load the object vector
obj_str=open(jsonFile).read() # read json object vector as a string
obj_vector.fillFromJson(obj_str) # populate the object vector

rob = obj_vector[0]
rob.lateReverb.levels = np.array( [0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 ], dtype = np.float32 )
rob.lateReverb.decayCoefficients = np.array( [-5.0, 1.0, 1.0, 1.0, -3.0, 1.0, 1.0, 1.0, 1.0 ], dtype = np.float32 )

# create signal flow context and loudspeaker array config
blockSize = 128
samplingFrequency = 48000
ctxt = visr.SignalFlowContext( blockSize, samplingFrequency) 
lc = panning.LoudspeakerArray( os.path.join( visrBaseDirectory, 'config/generic/bs2051-9+10+3.xml' ) )

robj=obj_vector[0]
lr=robj.lateReverb

lr_param=ro.LateReverbParameter(0,lr)

calc = ro.LateReverbFilterCalculator( ctxt, 'calc', None,
            numberOfObjects=1,
            lateReflectionLengthSeconds=1.0,
            numLateReflectionSubBandLevels=9,
            maxUpdatesPerPeriod=100)

flow = rrl.AudioSignalFlow( calc )

lr_input=flow.parameterReceivePort("subbandInput")
filter_out=flow.parameterSendPort("lateFilterOutput")

lr_input.enqueue(lr_param)

firTaps={}
flow.process() # Parameter only, no audio signal required.
while not filter_out.empty():
    filterStruct = filter_out.front()
    #print( "Received filter update for index %d in iteration %d." % (filterStruct.index, idx ) )
    # Retrieve the filter coefficients
    firTaps[filterStruct.index] = np.array(filterStruct.value)
    # TODO: Analyse the filters.
   
    # Slight oddity: We have to delete the Python variable because it would screw up the memory management
    # if the underlying C++ object is deleted in pop() while the Python representation is still alive.
       
    del filterStruct
 
    filter_out.pop()

plt.figure(1)
plt.plot(firTaps[0] )
