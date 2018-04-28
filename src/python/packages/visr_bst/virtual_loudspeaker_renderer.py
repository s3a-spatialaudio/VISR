# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import pml
import rbbl
import rcl

from .util.read_sofa_file import readSofaFile

from .virtual_loudspeaker_controller import VirtualLoudspeakerController

import numpy as np

class VirtualLoudspeakerRenderer( visr.CompositeComponent ):
    """
    Signal flow for rendering binaural output for a multichannel signal reproduced over a virtual loudspeaker array with corresponding BRIR data.
    """
    def __init__( self,
                 context, name, parent,              # Standard visr.Component construction arguments.
                 *,                                  # This ensures that the remaining arguments are given as keyword arguments.
                 sofaFile = None,                    # BRIR database provided as a SOFA file. This is an alternative to the hrirPosition, hrirData (and optionally hrirDelays) argument.
                 hrirPositions = None,               # Optional way to provide the measurement grid for the BRIR listener view directions. If a SOFA file is provided, this is optional and
                                                     # overrides the listener view data in the file. Otherwise this argument is mandatory. Dimension #grid directions x (dimension of position argument)
                 hrirData = None,                    # Optional way to provide the BRIR data. Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
                 hrirDelays = None,                  # Optional BRIR delays. If a SOFA file is given, this  argument overrides a potential delay setting from the file. Otherwise, no extra delays
                                                     # are applied unless this option is provided. Dimension: #grid directions  x #ears(2) x # loudspeakers

                 headOrientation = None,             # Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
                                                     # or the initial view direction
                 headTracking = True,                # Whether dynamic headTracking is active. If True, an control input "tracking" is created.
                 dynamicITD = False,                 # Whether the delay part of th BRIRs is applied separately to the (delay-free) BRIRs.
                 hrirInterpolation = False,          # Whether BRIRs are interpolated for the current head oriention. If False, a nearest-neighbour interpolation is used.
                 irTruncationLength = None,          # Maximum number of samples of the BRIR impulse responses. Functional only if the BRIR is provided in a SOFA file.
                 filterCrossfading = False,          # Whether dynamic BRIR changes are crossfaded (True) or switched immediately (False)
                 interpolatingConvolver = False,     # Whether the interpolating convolver option is used. If True, the convolver stores all BRIR filters, and the controller sends only
                                                     # interpolation coefficient messages to select the BRIR filters and their interpolation ratios.
                 fftImplementation = 'default'       # The FFT implementation to be used in the convolver. the default value selects the system default.
                 ):
        """
        Constructor.

        Parameters
        ----------
        context : visr.SignalFlowContext
            Standard visr.Component construction argument, a structure holding the block size and the sampling frequency
        name : string
            Name of the component, Standard visr.Component construction argument
        parent : visr.CompositeComponent
            Containing component if there is one, None if this is a top-level component of the signal flow.
        sofaFile: string
            BRIR database provided as a SOFA file. This is an alternative to the hrirPosition, hrirData
            (and optionally hrirDelays) argument. Default None means that hrirData and hrirPosition must be provided.
        hrirPositions : numpy.ndarray
            Optional way to provide the measurement grid for the BRIR listener view directions. If a
            SOFA file is provided, this is optional and overrides the listener view data in the file.
            Otherwise this argument is mandatory. Dimension #grid directions x (dimension of position argument)
        hrirData: numpy.ndarray
            Optional way to provide the BRIR data. Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
        hrirDelays: numpy.ndarray
            Optional BRIR delays. If a SOFA file is given, this  argument overrides a potential delay setting from the file. Otherwise, no extra delays
            are applied unless this option is provided. Dimension: #grid directions  x #ears(2) x # loudspeakers
        headOrientation : array-like
            Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
            or the initial view direction
        headTracking: bool
            Whether dynamic headTracking is active. If True, an control input "tracking" is created.
        dynamicITD: bool
            Whether the delay part of th BRIRs is applied separately to the (delay-free) BRIRs.
        hrirInterpolation: bool
            Whether BRIRs are interpolated for the current head oriention. If False, a nearest-neighbour interpolation is used.
        irTruncationLength: int
            Maximum number of samples of the BRIR impulse responses. Functional only if the BRIR is provided in a SOFA file.
        filterCrossfading: bool
            Whether dynamic BRIR changes are crossfaded (True) or switched immediately (False)
        interpolatingConvolver: bool
            Whether the interpolating convolver option is used. If True, the convolver stores all BRIR filters, and the controller sends only
            interpolation coefficient messages to select the BRIR filters and their interpolation ratios.
        fftImplementation: string
            The FFT implementation to be used in the convolver. the default value selects the system default.
        """
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

        # Check consistency between HRIR positions and HRIR data
        if (hrirPositions.shape[0] != hrirData.shape[0]):
            raise ValueError( "The number of HRIR positions is inconsistent with the dimension of the HRIR data." )

        # Additional safety check (is tested in the controller anyway)
        if dynamicITD:
            if (hrirDelays is None) or (hrirDelays.ndim != hrirData.ndim-1) or (hrirDelays.shape != hrirData.shape[0:-1] ):
                raise ValueError( 'If the "dynamicITD" option is given, the parameter "delays" must match the first dimensions of the hrir data matrix.' )

        self.virtualLoudspeakerController = VirtualLoudspeakerController( context, "VirtualLoudspeakerController", self,
                                                                  hrirPositions=hrirPositions,
                                                                  hrirData = hrirData,
                                                                  headTracking = headTracking,
                                                                  dynamicITD = dynamicITD,
                                                                  hrirInterpolation = hrirInterpolation,
                                                                  hrirDelays = hrirDelays,
                                                                  interpolatingConvolver=interpolatingConvolver
                                                                  )

        if headTracking:
            self.parameterConnection( self.trackingInput, self.virtualLoudspeakerController.parameterPort("headTracking"))


        # Define the routing for the binaural convolver such that it matches the organisation of the
        # flat BRIR matrix.
        filterRouting = rbbl.FilterRoutingList()

        firLength = hrirData.shape[-1]

        if dynamicITD:
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

                numFilters = np.prod( hrirData.shape[:-1])
                filterReshaped = np.reshape( hrirData, (numFilters,firLength))
                self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=2*numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=numFilters,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 numberOfInterpolants=2, # TODO: Find out from
                                                 transitionSamples=interpolationSteps,
                                                 filters = filterReshaped,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.InterpolatingFirFilterMatrix.ControlPortConfig.Filters,
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
                                                 controlInputs=rcl.CrossfadingFirFilterMatrix.ControlPortConfig.Filters,
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
                filterReshaped = np.reshape( hrirData, (numFilters, firLength ))
                self.convolver = rcl.InterpolatingFirFilterMatrix( context, 'convolutionEngine', self,
                                                 numberOfInputs=numberOfLoudspeakers,
                                                 numberOfOutputs=2,
                                                 maxFilters=numFilters,
                                                 filterLength=firLength,
                                                 maxRoutings=2*numberOfLoudspeakers,
                                                 numberOfInterpolants=2, # TODO: Find out from
                                                 transitionSamples=interpolationSteps,
                                                 filters = filterReshaped,
                                                 routings=filterRouting,
                                                 controlInputs=rcl.InterpolatingFirFilterMatrix.ControlPortConfig.Interpolants,
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
                                                 controlInputs=rcl.CrossfadingFirFilterMatrix.ControlPortConfig.Filters,
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
