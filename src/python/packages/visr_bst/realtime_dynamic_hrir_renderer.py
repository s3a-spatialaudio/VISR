# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import pml

from .dynamic_hrir_renderer import DynamicHrirRenderer

from .util.read_sofa_file import readSofaFile

class RealtimeDynamicHrirRenderer(visr.CompositeComponent ):
    def __init__( self,
                 context, name, parent,                 # Standard arguments for visr.Component constructors
                 *,                                     # No positional arguments beyond here.
                 numberOfObjects,                       # Maximum number of audio objects
                 sofaFile = None,                       # Whether a SOFA file is used to loaded the HRIR data.
                 hrirPositions = None,                  # Optional way to provide the measurement grid for the BRIR listener view directions. If a SOFA file is provided, this is optional and
                                                        # overrides the listener view data in the file. Otherwise this argument is mandatory. Dimension #grid directions x (dimension of position argument)
                 hrirData = None,                       # Optional way to provide the BRIR data. Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
                 hrirDelays = None,                     # Optional BRIR delays. If a SOFA file is given, this  argument overrides a potential delay setting from the file. Otherwise, no extra delays
                                                        # are applied unless this option is provided. Dimension: #grid directions  x #ears(2) x # loudspeakers
                 headOrientation = None,                # Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
                                                        # or the initial view direction
                 headTracking = True,                   # Whether dynamic racking is used.
                 dynamicITD = True,                     # Whether the ITD is applied separately. That requires preprocessed HRIR data
                 dynamicILD = True,                     # Whether the ILD is computed and applied separately. At the moment this feature is not used (apart from applying the object gains)
                 hrirInterpolation = True,              # Whether the controller supports interpolation between neighbouring HRTF gridpoints. False means nearest neighbour (no interpolation),
                                                        # True enables barycentric interpolation.
                 filterCrossfading = False,             # Use a crossfading FIR filter matrix to avoid switching artifacts.
                 fftImplementation = "default",         # The FFT implementation to use.
                 headTrackingReceiver = None,           # Class of the head tracking recveiver, None (default value) disables dynamic head tracking.
                 headTrackingPositionalArguments = None,# Positional arguments passed to the constructor of the head tracking receiver object. Must be a tuple.
                 headTrackingKeywordArguments = None    # Keyword arguments passed to the constructor of the head tracking receiver. Must be a dictionary (dict)
                 ):
        super( RealtimeDynamicHrirRenderer, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
        self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )

        enableTracking = (headTrackingReceiver is not None)

        # Handle loading of HRIR data from either a SOFA file or the the matrix arguments.
        if (hrirData is not None) == (sofaFile is not None):
            raise ValueError( "Exactly one of the arguments sofaFile and hrirData must be present." )
        if sofaFile is not None:
            [ sofaHrirPositions, hrirData, sofaHrirDelays ] = readSofaFile( sofaFile )
            # If hrirDelays is not provided as an argument, use the one retrieved from the SOFA file
            if hrirDelays is None:
                hrirDelays = sofaHrirDelays
            # Use the positions obtained from the SOFA file only if the argument is not set
            if hrirPositions is None:
                hrirPositions = sofaHrirPositions

        self.dynamicHrirRenderer = DynamicHrirRenderer( context, "DynamicBinauralRenderer", self,
                                                       numberOfObjects = numberOfObjects,
                                                       hrirPositions = hrirPositions,
                                                       hrirData = hrirData,
                                                       hrirDelays = hrirDelays,
                                                       headOrientation = headOrientation,
                                                       headTracking = enableTracking,
                                                       dynamicITD = dynamicITD,
                                                       dynamicILD = dynamicILD,
                                                       hrirInterpolation = hrirInterpolation,
                                                       filterCrossfading = filterCrossfading,
                                                       fftImplementation = fftImplementation
                                                       )

        if enableTracking:
            if headTrackingPositionalArguments == None:
                headTrackingPositionalArguments = ()
            if headTrackingKeywordArguments == None:
                headTrackingKeywordArguments = {}
            self.trackingDevice = headTrackingReceiver(context, "HeadTrackingReceiver", self,
                                                *headTrackingPositionalArguments,
                                                **headTrackingKeywordArguments )
            self.parameterConnection( self.trackingDevice.parameterPort("orientation"), self.dynamicBinauralRenderer.parameterPort("tracking"))

        self.parameterConnection( self.objectVectorInput, self.dynamicBinauralRenderer.parameterPort("objectVector"))
        self.audioConnection(  self.objectSignalInput, self.dynamicBinauralRenderer.audioPort("audioIn"))
        self.audioConnection( self.dynamicBinauralRenderer.audioPort("audioOut"), self.binauralOutput)
