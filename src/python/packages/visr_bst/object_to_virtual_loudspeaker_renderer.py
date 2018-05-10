# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import panning
import pml
import efl

from rcl import NullSource
from signalflows import CoreRenderer

from .virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer

import numpy as np

class ObjectToVirtualLoudspeakerRenderer( visr.CompositeComponent ):
    """
    Signal flow for rendering an object-based scene over a virtual loudspeaker binaural renderer.
    """
    def __init__( self,
                 context, name, parent,
                 *,                     # This ensures that the remaining arguments are given as keyword arguments.
                 numberOfObjects,
                 sofaFile = None,
                 hrirPositions = None,
                 hrirData = None,
                 hrirDelays = None,
                 headOrientation = None,
                 headTracking = True,
                 dynamicITD = False,
                 hrirInterpolation = False,
                 irTruncationLength = None,
                 filterCrossfading = False,
                 interpolatingConvolver = False,
                 fftImplementation = "default",
                 loudspeakerConfiguration = None,
                 loudspeakerRouting = None,
                 objectRendererOptions = {}
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
        loudspeakerConfiguration: panning.LoudspeakerArray
            Loudspeaker configuration object used in the ob ject renderer. Must not be None
        loudspeakerRouting: array-like list of integers or None
            Routing indices from the outputs of the object renderer to the inputs of the binaural virtual loudspeaker renderer.
            If empty, the outputs of the object renderer are connected to the first inputs of the virt. lsp renderer.
        objectRendererOptions: dict
            Keyword arguments passed to the object renderer (rcl.CoreRenderer). This may involve all optional
            arguments for this class apart from loudspeakerConfiguration, numberOfInputs, and numberOfOutputs
        """

        # Parameter checking
        if not isinstance( loudspeakerConfiguration, panning.LoudspeakerArray ):
            # Try to convert automatically
            loudspeakerConfiguration = panning.LoudspeakerArray( loudspeakerConfiguration )
            # raise ValueError( "'loudspeakerConfiguration' is not a 'panning.LoudspeakerArray' object." )
        numArraySpeakers = loudspeakerConfiguration.numberOfRegularLoudspeakers

        outRoutings = list(range(numArraySpeakers)) # Plain[0,1,...] routing

        super(ObjectToVirtualLoudspeakerRenderer,self).__init__( context, name, parent )

        self.objectInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
        self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )
        if headTracking:
            self.trackingInput = visr.ParameterInput( "tracking", self, pml.ListenerPosition.staticType,
                                          pml.DoubleBufferingProtocol.staticType,
                                          pml.EmptyParameterConfig() )

        objectRendererOptions["loudspeakerConfiguration"] = loudspeakerConfiguration
        objectRendererOptions["numberOfInputs"] = numberOfObjects
        objectRendererOptions["numberOfOutputs"] = numArraySpeakers

        if "interpolationPeriod" not in objectRendererOptions:
            objectRendererOptions["interpolationPeriod"] = context.period

        if "diffusionFilters" not in objectRendererOptions:
            diffLen = 512
            fftLen = int(np.ceil( 0.5*(diffLen+1) ))
            H = np.exp( -1j*(np.random.rand( numArraySpeakers, fftLen )) )
            h = np.fft.irfft( H, axis=1 )
            diffFilters = efl.BasicMatrixFloat( h )
            objectRendererOptions["diffusionFilters"] = diffFilters

        self.objectRenderer = CoreRenderer( context, "ObjectRenderer", self,
                                           **objectRendererOptions )

        self.virtualLoudspeakerRenderer = VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", self,
                 sofaFile=sofaFile,
                 hrirPositions=hrirPositions,
                 hrirData=hrirData,
                 hrirDelays=hrirDelays,
                 headOrientation=headOrientation,
                 headTracking=headTracking,
                 dynamicITD=dynamicITD,
                 hrirInterpolation=hrirInterpolation,
                 irTruncationLength=irTruncationLength,
                 filterCrossfading=filterCrossfading,
                 interpolatingConvolver=interpolatingConvolver,
                 fftImplementation=fftImplementation )

        self.audioConnection( self.objectInput, self.objectRenderer.audioPort("audioIn") )
        numVirtualSpeakers = self.virtualLoudspeakerRenderer.audioPort('audioIn').width

        if loudspeakerRouting is None:
            if numVirtualSpeakers != numArraySpeakers:
                raise ValueError( "If no 'loudspeakerRouting' parameter is provided, the numbers of loudspeakers of the object renderer and the binaural virt. loudspeaker renderer must match.")
            loudspeakerRouting = list(range(numArraySpeakers)) # Plain[0,1,...] routing

        if numVirtualSpeakers > numArraySpeakers:
            unconnectedSpeakers = list(set(range(numVirtualSpeakers)) - set(outRoutings) )
            self.nullSource = NullSource( context, "NullSource", self, width=1 )
            self.audioConnection( self.nullSource.audioPort("out"), [0]*len(unconnectedSpeakers),
                                 self.virtualLoudspeakerRenderer.audioPort("audioIn"),
                                 loudspeakerRouting )
        self.audioConnection( self.objectRenderer.audioPort("audioOut"), outRoutings,
                             self.virtualLoudspeakerRenderer.audioPort("audioIn"),
                             loudspeakerRouting )
        self.audioConnection( self.virtualLoudspeakerRenderer.audioPort("audioOut"), self.binauralOutput)
        self.parameterConnection( self.objectVectorInput, self.objectRenderer.parameterPort("objectDataInput") )
        if headTracking:
            self.parameterConnection( self.trackingInput, self.virtualLoudspeakerRenderer.parameterPort("tracking"))
