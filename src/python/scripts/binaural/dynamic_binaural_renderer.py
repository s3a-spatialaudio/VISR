#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Sep 13 12:56:27 2017

@author: gc1y17
"""

import visr
import pml
import rcl

#import objectmodel as om

from readSofa import readSofaFile
from dynamic_binaural_controller import DynamicBinauralController
import numpy as np

class DynamicBinauralRenderer( visr.CompositeComponent ):
    
        def __init__( self,
                     context, name, parent, 
                     numberOfObjects,sofaFile,
                     headTracking = True,
                     dynITD = True,
                     dynILD = True,
                     hrirInterp = True
                     ):
            super( DynamicBinauralRenderer, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
            if headTracking:
                self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            
         
           
            [ hrirPos, hrirData, delays ] = readSofaFile( sofaFile )  
            
            if dynITD:
                if (delays is None) or (delays.ndim != 2) or (delays.shape != (hrirData.shape[0], 2 ) ):
                    raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )
            
#            print(hrirPos)
#            print(delays)
#            
#            print(hrirPos[0])
            self.dynamicBinauraController = DynamicBinauralController( context, "DynamicBinauralController", self,
                                                                      numberOfObjects,
                                                                      hrirPos, hrirData,
                                                                      useHeadTracking = headTracking,
                                                                      dynamicITD = dynITD,
                                                                      dynamicILD = dynILD,
                                                                      hrirInterpolation = hrirInterp,
                                                                      delays = delays
                                                                      )
            
            self.parameterConnection( self.objectVectorInput, self.dynamicBinauraController.parameterPort("objectVector"))
            if headTracking:
                self.parameterConnection( self.trackingInput, self.dynamicBinauraController.parameterPort("headTracking"))
            
           
            # Define the routing for the binaural convolver such that it matches the organisation of the
            # flat BRIR matrix.
            filterRouting = pml.FilterRoutingList()
            for idx in range(0, numberOfObjects ):
                filterRouting.addRouting( idx, idx, idx, 1.0 )
                filterRouting.addRouting( idx, idx+numberOfObjects, idx+numberOfObjects, 1.0 )
                
            firLength = hrirData.shape[1]
            self.convolver = rcl.FirFilterMatrix( context, 'covolutionEngine', self )
            self.convolver.setup( numberOfInputs=numberOfObjects,
                             numberOfOutputs=2*numberOfObjects,
                             maxFilters=2*numberOfObjects,
                             filterLength=firLength,
                             maxRoutings=2*numberOfObjects,
                             routings=filterRouting,
                             controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters
                             )
            self.audioConnection(self.objectSignalInput, self.convolver.audioPort("in") )
            self.parameterConnection(self.dynamicBinauraController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )

            if dynITD:
                delayControls = rcl.DelayVector.ControlPortConfig.Delay
            else:
                delayControls = rcl.DelayVector.ControlPortConfig.No

            if dynILD:
                delayControls |= rcl.DelayVector.ControlPortConfig.Gain

            self.delayVector = rcl.DelayVector( context, "delayVector", self,
             numberOfObjects*2, interpolationType="lagrangeOrder3", initialDelay=0,
             controlInputs=delayControls,
             methodDelayPolicy=rcl.DelayMatrix.MethodDelayPolicy.Add,
             initialGain=1.0, 
             interpolationSteps=context.period)

            if dynITD:
                self.parameterConnection(self.dynamicBinauraController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )
            if dynILD:
                self.parameterConnection(self.dynamicBinauraController.parameterPort("gainOutput"),self.delayVector.parameterPort("gainInput") )            
            self.audioConnection( self.convolver.audioPort("out"), self.delayVector.audioPort("in"))

            self.adder = rcl.Add( context, 'add', self, numInputs = numberOfObjects, width=2)            

            for idx in range(0, numberOfObjects ):
                self.audioConnection( self.delayVector.audioPort("out"), visr.ChannelList([idx,idx+numberOfObjects]), self.adder.audioPort("in%d" % idx), visr.ChannelList([0,1]))                
                
                
            self.audioConnection( self.adder.audioPort("out"), self.binauralOutput)
           