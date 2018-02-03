#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 26 16:22:24 2017

@author: gc1y17
"""

import visr
import efl
import pml
import rbbl
import rcl

#import objectmodel as om

from readSofa import readSofaFile

from virtual_loudspeaker_controller import VirtualLoudspeakerController

import numpy as np

fftImplementation = 'ffts'
#fftImplementation = 'kissfft'
#fftImplementation = 'default'

class VirtualLoudspeakerRenderer( visr.CompositeComponent ):

        def __init__( self,
                     context, name, parent,
                     numberOfLoudspeakers,
                     sofaFile,
                     headTracking = True,
                     dynITD = False,
                     hrirInterp = False,
                     irTruncationLength = None,
                     filterCrossfading = False,
                     interpolatingConvolver = False
                     ):
            super( VirtualLoudspeakerRenderer, self ).__init__( context, name, parent )
            self.loudspeakerSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfLoudspeakers )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

            if headTracking:
                self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )

            [ hrirPos, hrirData, hrirDelays ] = readSofaFile( sofaFile,
                                                             truncationLength=irTruncationLength,
                                                             truncationWindowLength=16 )

            # Additional safety check (is tested in the controller anyway)
            if dynITD:
                if (hrirDelays is None) or (hrirDelays.ndim != hrirData.ndim-1) or (hrirDelays.shape != hrirData.shape[0:-1] ):
                    raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must match the first dimensions of the hrir data matrix.' )

            # The HRIR positions contained in the SOFA file are not the different head rotations, but the loudspeaker positions.
            # Therefore we assume a equidistant grid in the horizontal plane (conventions as in the Sound Scape Renderer)
            numHrirPos = hrirData.shape[0];
            hrirAz = np.arange( 0, 2*np.pi, (2.0*np.pi)/numHrirPos )
            hrirPos = np.stack( (hrirAz, np.ones(numHrirPos)), 1 )

            self.virtualLoudspeakerController = VirtualLoudspeakerController( context, "VirtualLoudspeakerController", self,
                                                                      numberOfLoudspeakers,
                                                                      hrirPos, hrirData,
                                                                      useHeadTracking = headTracking,
                                                                      dynamicITD = dynITD,
                                                                      hrirInterpolation = hrirInterp,
                                                                      delays = hrirDelays,
                                                                      interpolatingConvolver=interpolatingConvolver
                                                                      )

            if headTracking:
                self.parameterConnection( self.trackingInput, self.virtualLoudspeakerController.parameterPort("headTracking"))


            # Define the routing for the binaural convolver such that it matches the organisation of the
            # flat BRIR matrix.
            filterRouting = rbbl.FilterRoutingList()

            firLength = hrirData.shape[-1]

            if dynITD:
                self.delayVector = rcl.DelayVector( context, "delayVector", self,
                                                   numberOfLoudspeakers*2, interpolationType="lagrangeOrder3", initialDelay=0,
                                                   controlInputs=rcl.DelayVector.ControlPortConfig.Delay,
                                                   methodDelayPolicy=rcl.DelayMatrix.MethodDelayPolicy.Add,
                                                   initialGain=1.0,
                                                   interpolationSteps=context.period)

                self.audioConnection(self.loudspeakerSignalInput, [ i % numberOfLoudspeakers for i in range(numberOfLoudspeakers*2)],
                                     self.delayVector.audioPort("in"), range(0,2*numberOfLoudspeakers ) )

                for idx in range(0, numberOfLoudspeakers ):
                    filterRouting.addRouting( idx, 0, idx, 1.0 )
                    filterRouting.addRouting( idx+numberOfLoudspeakers, 1, idx+numberOfLoudspeakers, 1.0 )

                if interpolatingConvolver:
                    if filterCrossfading:
                        interpolationSteps = context.period
                    else:
                        interpolationSteps = 0

                    filterReshaped = np.concatenate( (hrirData[:,0,...],hrirData[:,1,...]), axis=1 )
                    filterMtx = efl.BasicMatrixFloat(filterReshaped)
                    self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=2*numberOfLoudspeakers,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfLoudspeakers,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfLoudspeakers,
                                                     numberOfInterpolants=2, # TODO: Find out from
                                                     transitionSamples=interpolationSteps,
                                                     filters = filterMtx,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                     fftImplementation=fftImplementation
                                                     )

                elif filterCrossfading:
                    self.convolver = rcl.CrossfadingFirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=2*numberOfLoudspeakers,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfLoudspeakers,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfLoudspeakers,
                                                     transitionSamples=context.period,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                     fftImplementation=fftImplementation
                                                     )
                else:
                    self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                 numberOfInputs=2*numberOfLoudspeakers,
                                 numberOfOutputs=2,
                                 maxFilters=2*numberOfLoudspeakers,
                                 filterLength=firLength,
                                 maxRoutings=2*numberOfLoudspeakers,
                                 routings=filterRouting,
                                 controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                 fftImplementation=fftImplementation
                                 )

                self.audioConnection( self.delayVector.audioPort("out"), self.convolver.audioPort("in"), )
                self.parameterConnection(self.virtualLoudspeakerController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )

            else: # no dynamic ITD
                for idx in range(0, numberOfLoudspeakers ):
                    filterRouting.addRouting( idx, 0, idx, 1.0 )
                    filterRouting.addRouting( idx, 1, idx+numberOfLoudspeakers, 1.0 )
                if interpolatingConvolver:
                    if filterCrossfading:
                        interpolationSteps = context.period
                    else:
                        interpolationSteps = 0

                    #filterReshaped = np.concatenate( (hrirData[:,0,...],hrirData[:,1,...]), axis=1 )
                    numFilters = np.prod(np.array(hrirData.shape[0:-1]))
                    filterReshaped = np.reshape( hrirData, (numfilters, hrirData.shape[0:-1] ))
                    filterMtx = efl.BasicMatrixFloat(filterReshaped)
                    self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=numberOfLoudspeakers,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfLoudspeakers,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfLoudspeakers,
                                                     numberOfInterpolants=2, # TODO: Find out from
                                                     transitionSamples=interpolationSteps,
                                                     filters = filterMtx,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                     fftImplementation=fftImplementation
                                                     )
                elif filterCrossfading:
                    self.convolver = rcl.CrossfadingFirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=numberOfLoudspeakers,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfLoudspeakers,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfLoudspeakers,
                                                     transitionSamples=context.period,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                     fftImplementation=fftImplementation
                                                     )
                else:
                    self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=numberOfLoudspeakers,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfLoudspeakers,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfLoudspeakers,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                     fftImplementation=fftImplementation
                                                     )
                self.audioConnection(self.loudspeakerSignalInput,
                                     self.convolver.audioPort("in") )

            if interpolatingConvolver:
                self.parameterConnection(self.virtualLoudspeakerController.parameterPort("interpolatorOutput"),self.convolver.parameterPort("interpolantInput") )
            else:
                self.parameterConnection(self.virtualLoudspeakerController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )
            self.audioConnection( self.convolver.audioPort("out"), self.binauralOutput)
