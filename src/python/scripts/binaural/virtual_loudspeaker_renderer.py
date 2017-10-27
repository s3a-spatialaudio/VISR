#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 26 16:22:24 2017

@author: gc1y17
"""

import visr
import pml
import rcl

#import objectmodel as om

from readSofa import readSofaFileBRIR
from virtual_loudspeaker_controller import VirtualLoudspeakerController

class VirtualLoudspeakerRenderer( visr.CompositeComponent ):
    
        def __init__( self,
                     context, name, parent, 
                     numberOfLoudspeakers,
                     sofaFile,
                     headTracking = True,
                     dynITD = False,
                     dynILD = False,
                     hrirInterp = False
                     ):
            super( VirtualLoudspeakerRenderer, self ).__init__( context, name, parent )
            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfLoudspeakers )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
           
            if headTracking:
                self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
            
         
           
            [ hrirPos, hrirData, delays ] = readSofaFileBRIR( sofaFile )  
            
            if dynITD:
                if (delays is None) or (delays.ndim != 2) or (delays.shape != (hrirData.shape[0], 2 ) ):
                    raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )
            
#            print(hrirPos)
#            print(delays)
#            
#            print(hrirPos[0])
            self.virtualLoudspeakerController = VirtualLoudspeakerController( context, "VirtualLoudspeakerController", self,
                                                                      numberOfLoudspeakers,
                                                                      hrirPos, hrirData,
                                                                      useHeadTracking = headTracking,
                                                                      dynamicITD = dynITD,
                                                                      dynamicILD = dynILD,
                                                                      hrirInterpolation = hrirInterp,
                                                                      delays = delays
                                                                      )
            
            if headTracking:
                self.parameterConnection( self.trackingInput, self.virtualLoudspeakerController.parameterPort("headTracking"))
            
           
            # Define the routing for the binaural convolver such that it matches the organisation of the
            # flat BRIR matrix.
            filterRouting = pml.FilterRoutingList()
            for idx in range(0, numberOfLoudspeakers ):
                filterRouting.addRouting( idx, idx, idx, 1.0 )
                filterRouting.addRouting( idx, idx+numberOfLoudspeakers, idx+numberOfLoudspeakers, 1.0 )
                
            firLength = hrirData.shape[1]
            self.convolver = rcl.FirFilterMatrix( context, 'covolutionEngine', self )
            self.convolver.setup( numberOfInputs=numberOfLoudspeakers,
                                 numberOfOutputs=2*numberOfLoudspeakers,
                                 maxFilters=2*numberOfLoudspeakers,
                                 filterLength=firLength,
                                 maxRoutings=2*numberOfLoudspeakers,
                                 routings=filterRouting,
                                 controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters
                                 )
          
            self.audioConnection(self.objectSignalInput, self.convolver.audioPort("in") )
            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )


            self.delayVector = rcl.DelayVector( context, "delayVector", self )
            self.delayVector.setup(numberOfLoudspeakers*2, interpolationType="lagrangeOrder3", initialDelay=0,
             controlInputs=True, 
             methodDelayPolicy=rcl.DelayMatrix.MethodDelayPolicy.Add,
             initialGain=1.0, 
             interpolationSteps=context.period)

           
            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )
            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("gainOutput"),self.delayVector.parameterPort("gainInput") )            
            self.audioConnection( self.convolver.audioPort("out"), self.delayVector.audioPort("in"))

            self.adder = rcl.Add( context, 'add', self, numInputs = numberOfLoudspeakers, width=2)            


            for idx in range(0, numberOfLoudspeakers ):
                self.audioConnection( self.delayVector.audioPort("out"), visr.ChannelList([idx,idx+numberOfLoudspeakers]), self.adder.audioPort("in%d" % idx), visr.ChannelList([0,1]))                
                
                
            self.audioConnection( self.adder.audioPort("out"), self.binauralOutput)