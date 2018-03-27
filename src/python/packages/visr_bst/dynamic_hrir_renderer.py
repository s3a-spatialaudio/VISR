# %BST_LICENCE_TEXT%

import numpy as np

import visr
import pml
import rbbl
import rcl

from .util.read_sofa_file import readSofaFile
from .dynamic_hrir_controller import DynamicHrirController

class DynamicHrirRenderer( visr.CompositeComponent ):

    def __init__( self,
             context, name, parent,     # Standard arguments for a VISR component
             numberOfObjects,           # Number of audo objects to be rendered.
             *,                         # No positional arguments beyond this point
             sofaFile = None,           # whether a SOFA file is used to loaded the HRIR data.
             hrirPositions = None,      # Optional way to provide the measurement grid for the BRIR listener view directions. If a SOFA file is provided, this is optional and
                                        # overrides the listener view data in the file. Otherwise this argument is mandatory. Dimension #grid directions x (dimension of position argument)
             hrirData = None,           # Optional way to provide the BRIR data. Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
             hrirDelays = None,         # Optional BRIR delays. If a SOFA file is given, this  argument overrides a potential delay setting from the file. Otherwise, no extra delays
                                        # are applied unless this option is provided. Dimension: #grid directions  x #ears(2) x # loudspeakers
             headOrientation = None,    # Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
                                        # or the initial view direction
             headTracking = True,       # Whether dynamic racking is used.
             dynamicITD = True,         # Whether the ITD is applied separately. That requires preprocessed HRIR data
             dynamicILD = True,         # Whether the ILD is computed and applied separately. At the moment this feature is not used (apart from applying the object gains)
             hrirInterpolation = True,  # Whether the controller supports interpolation between neighbouring HRTF gridpoints. False means nearest neighbour (no interpolation),
                                        # True enables barycentric interpolation.
             filterCrossfading = False, # Use a crossfading FIR filter matrix to avoid switching artifacts.
             interpolatingConvolver = False,
             fftImplementation = "default" # The FFT implementation to use.
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

        if (hrirData is not None) and (sofaFile is not None):
            raise ValueError( "Exactly one of the arguments sofaFile and hrirData must be present." )
        if sofaFile is not None:
            # We don't support HRIR truncation here because they are usually quite short.
            [ sofaHrirPositions, hrirData, sofaHrirDelays ] = readSofaFile( sofaFile )
            # If hrirDelays is not provided as an argument, use the one retrieved from the SOFA file
            if hrirDelays is None:
                hrirDelays = sofaHrirDelays
            # Use the positions obtained from the SOFA file only if the argument is not set
            if hrirPositions is None:
                hrirPositions = sofaHrirPositions

        if dynamicITD:
            if (hrirDelays is None) or (hrirDelays.ndim != 2) or (hrirDelays.shape != (hrirData.shape[0], 2 ) ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must be a #hrirs x 2 matrix.' )

        self.dynamicHrirController = DynamicHrirController( context, "DynamicHrirController", self,
                                                                  numberOfObjects,
                                                                  hrirPositions, hrirData,
                                                                  useHeadTracking = headTracking,
                                                                  dynamicITD = dynamicITD,
                                                                  dynamicILD = dynamicILD,
                                                                  hrirInterpolation = hrirInterpolation,
                                                                  interpolatingConvolver = interpolatingConvolver,
                                                                  hrirDelays = hrirDelays
                                                                  )

        self.parameterConnection( self.objectVectorInput, self.dynamicHrirController.parameterPort("objectVector"))
        if headTracking:
            self.parameterConnection( self.trackingInput, self.dynamicHrirController.parameterPort("headTracking"))

        firLength = hrirData.shape[-1]

        # Used if the InterpolatingConvolver is selected.
        numberOfInterpolants = 3 if hrirInterpolation else 1
        interpolationSteps = context.period if filterCrossfading else 0

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
                                 self.delayVector.audioPort("in"), range(2*numberOfObjects) )

            # Define the routing for the binaural convolver such that it match the layout of the
            # flat BRIR matrix.
            filterRouting = rbbl.FilterRoutingList()
            for idx in range(0, numberOfObjects ):
                filterRouting.addRouting( idx, 0, idx, 1.0 )
                filterRouting.addRouting( idx+numberOfObjects, 1, idx+numberOfObjects, 1.0 )
            numMatrixInputs = 2*numberOfObjects
        else:
            filterRouting = rbbl.FilterRoutingList()
            for idx in range(0, numberOfObjects ):
                filterRouting.addRouting( idx, 0, idx, 1.0 )
                filterRouting.addRouting( idx, 1, idx+numberOfObjects, 1.0 )
            filterRouting2 = rbbl.FilterRoutingList([ rbbl.FilterRouting(i%numberOfObjects, i//numberOfObjects, i, 1.0)
                              for i in range(2*numberOfObjects) ])
            numMatrixInputs = numberOfObjects

        if interpolatingConvolver:
            numFilters = np.prod(np.array(hrirData.shape[0:-1]))
            filterReshaped = np.reshape( hrirData, (numFilters, firLength ), 'C' )
            self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                             numberOfInputs=numMatrixInputs,
                                             numberOfOutputs=2,
                                             maxFilters=numFilters,
                                             filterLength=firLength,
                                             maxRoutings=2*numberOfObjects,
                                             numberOfInterpolants=numberOfInterpolants,
                                             transitionSamples=interpolationSteps,
                                             filters = filterReshaped,
                                             routings=filterRouting,
                                             controlInputs=rcl.InterpolatingFirFilterMatrix.ControlPortConfig.Interpolants,
                                             fftImplementation=fftImplementation
                                             )
        elif filterCrossfading:
            self.convolver = rcl.CrossfadingFirFilterMatrix( context, 'convolutionEngine', self,
                                                            numberOfInputs=numMatrixInputs,
                                                            numberOfOutputs=2,
                                                            maxFilters=2*numberOfObjects,
                                                            filterLength=firLength,
                                                            maxRoutings=2*numberOfObjects,
                                                            routings=filterRouting,
                                                            transitionSamples=context.period,
                                                            controlInputs=rcl.CrossfadingFirFilterMatrix.ControlPortConfig.Filters,
                                                            fftImplementation=fftImplementation
                                                            )
        else:
            self.convolver = rcl.FirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=numMatrixInputs,
                                                 numberOfOutputs=2,
                                                 maxFilters=2*numberOfObjects,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfObjects,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.FirFilterMatrix.ControlPortConfig.Filters,
                                                 fftImplementation=fftImplementation
                                                 )
        if dynamicITD or dynamicILD:
            self.audioConnection(self.delayVector.audioPort("out"), self.convolver.audioPort("in") )
            if dynamicITD:
                self.parameterConnection(self.dynamicHrirController.parameterPort("delayOutput"),
                                         self.delayVector.parameterPort("delayInput") )
            if dynamicILD:
                self.parameterConnection(self.dynamicHrirController.parameterPort("gainOutput"),
                                         self.delayVector.parameterPort("gainInput") )
        else:
            self.audioConnection(self.objectSignalInput, self.convolver.audioPort("in") )

        self.audioConnection(self.convolver.audioPort("out"), self.binauralOutput )
        if interpolatingConvolver:
            self.parameterConnection(self.dynamicHrirController.parameterPort("interpolatorOutput"),
                                     self.convolver.parameterPort("interpolantInput") )
        else:
            self.parameterConnection(self.dynamicHrirController.parameterPort("filterOutput"),
                                     self.convolver.parameterPort("filterInput") )
