#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Sep 13 12:56:27 2017

@author: gc1y17
"""

import visr
import pml
import rbbl
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

            self.dynamicBinauralController = DynamicBinauralController( context, "DynamicBinauralController", self,
                                                                      numberOfObjects,
                                                                      hrirPos, hrirData,
                                                                      useHeadTracking = headTracking,
                                                                      dynamicITD = dynITD,
                                                                      dynamicILD = dynILD,
                                                                      hrirInterpolation = hrirInterp,
                                                                      delays = delays
                                                                      )

            self.parameterConnection( self.objectVectorInput, self.dynamicBinauralController.parameterPort("objectVector"))
            if headTracking:
                self.parameterConnection( self.trackingInput, self.dynamicBinauralController.parameterPort("headTracking"))

            firLength = hrirData.shape[1]

            if dynITD or dynILD:
                if dynITD:
                    delayControls = rcl.DelayVector.ControlPortConfig.Delay
                else:
                    delayControls = rcl.DelayVector.ControlPortConfig.No
                if dynILD:
                    delayControls = delayControls | rcl.DelayVector.ControlPortConfig.Gain
                    initialGain = 0.0 # If the ILD is applied in the DelayVector, start from zero.
                else:
                    initialGain = 1.0 # Fixed setting as the gain of the delay vector is not used

                self.delayVector = rcl.DelayVector( context, "delayVector", self,
                                                   numberOfObjects*2, interpolationType="lagrangeOrder3", initialDelay=0,
                                                   controlInputs=delayControls,
                                                   methodDelayPolicy=rcl.DelayMatrix.MethodDelayPolicy.Add,
                                                   initialGain=initialGain,
                                                   interpolationSteps=context.period)

                inConnections = [ i % numberOfObjects for i in range(numberOfObjects*2)]
                self.audioConnection(self.objectSignalInput, inConnections,
                                     self.delayVector.audioPort("in"), range(0,2*numberOfObjects ) )

                # Define the routing for the binaural convolver such that it match the layout of the
                # flat BRIR matrix.
                filterRouting = pml.FilterRoutingList()
                for idx in range(0, numberOfObjects ):
                    filterRouting.addRouting( idx, 0, idx, 1.0 )
                    filterRouting.addRouting( idx+numberOfObjects, 1, idx+numberOfObjects, 1.0 )
                self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=2*numberOfObjects,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfObjects,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfObjects,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters
                                                     )
                self.audioConnection(self.delayVector.audioPort("out"), self.convolver.audioPort("in") )

                if dynITD:
                    self.parameterConnection(self.dynamicBinauralController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )
                if dynILD:
                    self.parameterConnection(self.dynamicBinauralController.parameterPort("gainOutput"),self.delayVector.parameterPort("gainInput") )
            else: # Neither dynILD or dynITD, that means no separate DelayVector
                filterRouting = pml.FilterRoutingList()
                for idx in range(0, numberOfObjects ):
                    filterRouting.addRouting( idx, 0, idx, 1.0 )
                    filterRouting.addRouting( idx, 1, idx+numberOfObjects, 1.0 )
                self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=numberOfObjects,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfObjects,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfObjects,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters
                                                     )
                self.audioConnection(self.objectSignalInput, self.convolver.audioPort("in") )

            self.audioConnection(self.convolver.audioPort("out"), self.binauralOutput )
            self.parameterConnection(self.dynamicBinauralController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )
