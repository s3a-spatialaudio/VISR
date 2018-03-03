#!/usr/bin/env python3

# %BST_LICENCE_TEXT%

import visr
import pml
import rcl

from .util.read_sofa_file import readSofaFile
from .dynamic_hrir_controller import DynamicHrirController

class DynamicHrirRenderer( visr.CompositeComponent ):

    def __init__( self,
             context, name, parent,
             numberOfObjects,sofaFile,
             headTracking = True,
             dynamicITD = True,
             dynamicILD = True,
             hrirInterpolation = True
             ):
        super( DynamicHrirRenderer, self ).__init__( context, name, parent )
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

        if dynamicITD:
            if (delays is None) or (delays.ndim != 2) or (delays.shape != (hrirData.shape[0], 2 ) ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )

        self.dynamicHrirController = DynamicHrirController( context, "DynamicHrirController", self,
                                                                  numberOfObjects,
                                                                  hrirPos, hrirData,
                                                                  useHeadTracking = headTracking,
                                                                  dynamicITD = dynamicITD,
                                                                  dynamicILD = dynamicILD,
                                                                  hrirInterpolation = hrirInterpolation,
                                                                  delays = delays
                                                                  )

        self.parameterConnection( self.objectVectorInput, self.DynamicHrirController.parameterPort("objectVector"))
        if headTracking:
            self.parameterConnection( self.trackingInput, self.DynamicHrirController.parameterPort("headTracking"))

        firLength = hrirData.shape[1]

        if dynamicITD or dynamicILD:
            if dynamicITD:
                delayControls = rcl.DelayVector.ControlPortConfig.Delay
            else:
                delayControls = rcl.DelayVector.ControlPortConfig.No
            if dynamicILD:
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

            if dynamicITD:
                self.parameterConnection(self.dynamicHrirController.parameterPort("delayOutput"),self.delayVector.parameterPort("delayInput") )
            if dynamicILD:
                self.parameterConnection(self.dynamicHrirController.parameterPort("gainOutput"),self.delayVector.parameterPort("gainInput") )
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
        self.parameterConnection(self.dynamicHrirController.parameterPort("filterOutput"),self.convolver.parameterPort("filterInput") )
