# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import rcl

from .virtual_loudspeaker_renderer import VirtualLoudspeakerRenderer

from .util.read_sofa_file import readSofaFile

# To be removed as soon as the tracking device is passed as a constructor parameter
# from .tracker.razor_ahrs import RazorAHRS

class RealtimeVirtualLoudspeakerRenderer(visr.CompositeComponent ):
        def __init__( self,
                     context, name, parent,
                     *,                                  # No positional arguments past this point.
                     sofaFile = None,                    # BRIR database provided as a SOFA file. This is an alternative to the hrirPosition, hrirData (and optionally hrirDelays) argument.
                     hrirPositions = None,               # Optional way to provide the measurement grid for the BRIR listener view directions. If a SOFA file is provided, this is optional and
                                                         # overrides the listener view data in the file. Otherwise this argument is mandatory. Dimension #grid directions x (dimension of position argument)
                     hrirData = None,                    # Optional way to provide the BRIR data. Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
                     hrirDelays = None,                  # Optional BRIR delays. If a SOFA file is given, this  argument overrides a potential delay setting from the file. Otherwise, no extra delays
                                                         # are applied unless this option is provided. Dimension: #grid directions  x #ears(2) x # loudspeakers

                     headOrientation = None,             # Head orientation in spherical coordinates (2- or 3-element vector or list). Either a static orientation (when no tracking is used),
                                                         # or the initial view direction
                     dynamicITD = True,
                     hrirInterpolation = True,
                     irTruncationLength = None,
                     headTrackingCalibrationPort = None,
                     filterCrossfading = False,
                     interpolatingConvolver = False,
                     headTrackingReceiver = None,
                     headTrackingPositionalArguments = None,
                     headTrackingKeywordArguments = None,
                     fftImplementation = 'default'
                     ):
            super( RealtimeVirtualLoudspeakerRenderer, self ).__init__( context, name, parent )

            # Handle loading of HRIR data from either a SOFA file or the the matrix arguments.
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

            self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfLoudspeakers )
            self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

            self.virtualLoudspeakerRenderer =  VirtualLoudspeakerRenderer( context, "VirtualLoudspeakerRenderer", self,
                                                           hrirPositions = hrirPositions,
                                                           hrirData = hrirData,
                                                           hrirDelays = hrirDelays,
                                                           headOrientation = None,
                                                           headTracking = (headTrackingReceiver is not None),
                                                           dynamicITD = dynamicITD,
                                                           hrirInterpolation = hrirInterpolation,
                                                           irTruncationLength = irTruncationLength,
                                                           filterCrossfading = filterCrossfading,
                                                           interpolatingConvolver = interpolatingConvolver,
                                                           fftImplementation = fftImplementation
                                                           )
            if headTrackingReceiver is not None:
                if headTrackingPositionalArguments == None:
                    headTrackingPositionalArguments = ()
                if headTrackingKeywordArguments == None:
                    headTrackingKeywordArguments = {}
                self.trackingDevice = headTrackingReceiver(context, "HeadTrackingReceiver", self,
                                                    *headTrackingPositionalArguments,
                                                    **headTrackingKeywordArguments )
                self.parameterConnection( self.trackingDevice.parameterPort("orientation"), self.virtualLoudspeakerRenderer.parameterPort("tracking"))

            self.audioConnection(  self.objectSignalInput, self.virtualLoudspeakerRenderer.audioPort("audioIn"))
            self.audioConnection( self.virtualLoudspeakerRenderer.audioPort("audioOut"), self.binauralOutput)
