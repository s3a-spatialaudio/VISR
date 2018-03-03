# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import pml
import rcl

#import objectmodel as om

from .util.read_sofa_file import readSofaFile

from .virtual_loudspeaker_controller import VirtualLoudspeakerController

import numpy as np

class VirtualLoudspeakerRenderer( visr.CompositeComponent ):
        def __init__( self,
                     context, name, parent,
                     sofaFile = None,
                     hrirPositions = None,
                     hrirData = None,
                     hrirDelays = None,
                     headOrientation = None,
                     headTracking = True,
                     dynITD = False,
                     hrirInterp = False,
                     irTruncationLength = None
                     ):
            if (hrirData is not None) and (sofaFile is not None):
                raise ValueError( "Exactly one of the arguments sofaFile and hrirData must be present." )
            if sofaFile is not None:
                [ sofaHrirPositions, hrirData, sofaHrirDelays ] = readSofaFile( sofaFile,
                                                             truncationLength=irTruncationLength,
                                                             truncationWindowLength=16 )
                # If hrirDelays is not provided as an argument, use the one retrieved from the SOFA file
                if hrirDelays is None:
                    hrirDelays = sofaHrirDelays
                # Use the positions obtained from the SOFA file only if the argument is not set
                if hrirPositions is None:
                    hrirPositions = sofaHrirPositions

            numberOfLoudspeakers = hrirData.shape[2]

            super(VirtualLoudspeakerRenderer,self).__init__( context, name, parent )
            self.loudspeakerSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfLoudspeakers )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
            if headTracking:
                self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )

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
                                                                      delays = hrirDelays
                                                                      )

            if headTracking:
                self.parameterConnection( self.trackingInput, self.virtualLoudspeakerController.parameterPort("headTracking"))


            # Define the routing for the binaural convolver such that it matches the organisation of the
            # flat BRIR matrix.
            filterRouting = pml.FilterRoutingList()

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
                self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=2*numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=2*numberOfLoudspeakers,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters
                                                 )
                self.audioConnection( self.delayVector.audioPort("out"), self.convolver.audioPort("in"), )
                self.parameterConnection(self.virtualLoudspeakerController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )

            else: # no dynamic ITD
                for idx in range(0, numberOfLoudspeakers ):
                    filterRouting.addRouting( idx, 0, idx, 1.0 )
                    filterRouting.addRouting( idx, 1, idx+numberOfLoudspeakers, 1.0 )
                self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                     numberOfInputs=numberOfLoudspeakers,
                                                     numberOfOutputs=2,
                                                     maxFilters=2*numberOfLoudspeakers,
                                                     filterLength=firLength,
                                                     maxRoutings=2*numberOfLoudspeakers,
                                                     routings=filterRouting,
                                                     controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters
                                                     )
                self.audioConnection(self.loudspeakerSignalInput,
                                     self.convolver.audioPort("in") )

            self.parameterConnection(self.virtualLoudspeakerController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )
            self.audioConnection( self.convolver.audioPort("out"), self.binauralOutput)
