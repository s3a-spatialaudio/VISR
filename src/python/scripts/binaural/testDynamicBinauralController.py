# -*- coding: utf-8 -*-
"""
Created on Fri Sep  8 08:16:32 2017

@author: af5u13
"""

from readSofa import readSofaFile
from dynamic_binaural_controller import DynamicBinauralController
from rotationFunctions import sph2cart, cart2sph, rad2deg, deg2rad
import visr
import objectmodel as om
import rrl
import numpy as np
import os
from urllib.request import urlretrieve

fs = 48000
bufferSize = 256

numBinauralObjects = 12

context = visr.SignalFlowContext( period=bufferSize, samplingFrequency=fs)

sofaFile = './data/dtf b_nh169.sofa'

if not os.path.exists( sofaFile ):
    urlretrieve( 'http://sofacoustics.org/data/database/ari%20(artificial)/dtf%20b_nh169.sofa',
                       sofaFile )

# hrirFile = 'c:/local/s3a_af/subprojects/binaural/dtf b_nh169.sofa'

[ hrirPos, hrirData ] = readSofaFile( sofaFile )
#print( "positions: %s." % str(np.array(hrirPos)))

headTrackEnabled = True

controller = DynamicBinauralController( context, "Controller", None,
                  numBinauralObjects,
                  hrirPos, hrirData,
                  useHeadTracking = headTrackEnabled,
                  dynamicITD = True,
                  dynamicILD = True,
                  hrirInterpolation = True
                  )

flow = rrl.AudioSignalFlow( controller )

objectInput = flow.parameterReceivePort( "objectVector" )

if headTrackEnabled:
    trackingInput = flow.parameterReceivePort( "headTracking" )

filterOutput = flow.parameterSendPort("filterOutput")

gainOutput = flow.parameterSendPort("gainOutput")


numPos = 72
azSequence = (2.0*np.pi)/numPos *  np.arange( 0, numPos )

for blockIdx in range(0,numPos):
    
    az = azSequence[blockIdx]
    x = np.cos( az )
    y = np.sin( az )
    z = 0
   
#    # 3D movement
#    az = deg2rad(blockIdx*5)
#    el = deg2rad(blockIdx*5)
#    x = np.cos( az )*np.sin(el)
#    y = np.sin( az )*np.sin(el)
#    z = np.cos(el)
    
    
    ps1 = om.PointSource( 0 ) 
    ps1.position = [x,y,z]
    ps1.level = 0.25
    ps1.channels = [0]
    
    ps2 = om.PointSource( 1 )
    ps2.position = [-x,-y,z]
    ps2.level = 0.25
    ps2.channels = [1]
    
    objectInput.data().assign( om.ObjectVector([ps1, ps2]) )
    objectInput.swapBuffers()
    
    if headTrackEnabled:
      headrotation =  np.pi/2;
      print("it num"+str(blockIdx)+", deg shift: "+str(rad2deg(azSequence[blockIdx]))+" head rotation: "+str(rad2deg(headrotation)))
   
      trackingInput.data().orientation = [0,0, -headrotation] #rotates over the z axis, that means that the rotation is on the xy plane
      trackingInput.swapBuffers()                      
    posSph1 = cart2sph(x,y,z)
    posSph2 = cart2sph(-x,-y,-z)
#    print('\ndeg shift: %d \np1 [%d %d] p2 [%d %d]' % (blockIdx*5,round(rad2deg(posSph1[0])),round(rad2deg(posSph1[1])),round(rad2deg(posSph2[0])),round(rad2deg(posSph2[1]))))
    print('\ndeg shift: %d' % (rad2deg(azSequence[blockIdx])))    
    flow.process()
    
    while not filterOutput.empty():
        filterSet = filterOutput.front()
#        print( "Received filter update for filter index %d." % filterSet.index )
        filterOutput.pop()
        del filterSet # Python garbage collection is somewhat subtle, so we try to
        # remove the reference explicitly.

    if gainOutput.changed():
        #print( "Updated hrir gains: %s." % str(np.array(gainOutput.data()) ))
        gainOutput.resetChanged()
