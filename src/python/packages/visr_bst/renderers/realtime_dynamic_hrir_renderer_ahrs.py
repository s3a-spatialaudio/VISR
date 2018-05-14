# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import pml
import rcl

from .dynamic_hrir_renderer import DynamicHrirRenderer

from .util.read_sofa_file import readSofaFile

#from .tracker.ahrs_tracker_with_udp_calibration import AhrsTrackerWithUdpCalibration
from .tracker.razor_ahrs_with_udp_calibration_trigger import RazorAHRSWithUdpCalibrationTrigger

class RealtimeDynamicHrirRendererAhrs(visr.CompositeComponent ):
    """
    VISR component for realtime audio rendering of object-based scenes using a 'dynamic HRIR' approach.

    It contains a DynamicHrirRenderer component, but optionally adds a receiver
    component for head tracking devices and real-time receipt of object metadata
    from UDP network packets.
    """
    def __init__( self,
                 context, name, parent,
                 *,
                 numberOfObjects,
                 sofaFile = None,
                 hrirPositions = None,
                 hrirData = None,
                 hrirDelays = None,
                 headOrientation = None,
                 dynamicITD = False,
                 dynamicILD = False,
                 hrirInterpolation = False,
                 filterCrossfading = False,
                 fftImplementation = "default",
                 headTracking = False,
                 headTrackingPositionalArguments = None,
                 headTrackingKeywordArguments = None,
                 sceneReceiveUdpPort = None
                 ):
        """
        Constructor.

        Parameters
        ----------
        context : visr.SignalFlowContext
            Standard visr.Component construction argument, holds the block size and the sampling frequency
        name : string
            Name of the component, Standard visr.Component construction argument
        parent : visr.CompositeComponent
            Containing component if there is one, None if this is a top-level component of the signal flow.
        numberOfObjects: int
            Maximum number of audio objects
        sofaFile: str, optional
            Optional SOFA for loading loaded the HRIR and associated data (HRIR measurement positions and delays)
            If not provided, the information must be provided by the hrirPositions and hrirData arguments.
        hrirPositions: numpy.ndarray, optional
            Optional way to provide the measurement grid for the BRIR listener view directions.
            If a SOFA file is provided, this is optional and overrides the listener view data
            in the file. Otherwise this argument is mandatory.
            Dimension #grid directions x (dimension of position argument)
        hrirData: numpy.ndarray, optional
            Optional way to provide the BRIR data.
            Dimension: #grid directions  x #ears (2) # x #loudspeakers x #ir length
        hrirDelays: numpy.ndarray, optional
            Optional BRIR delays. If a SOFA file is given, this  argument overrides
            a potential delay setting from the file. Otherwise, no extra delays
            are applied unless this option is provided.
            Dimension: #grid directions  x #ears(2) x # loudspeakers
        headOrientation: array-like, optional
            Head orientation in spherical coordinates (2- or 3-element vector or list).
            Either a static orientation (when no tracking is used), or the
            initial view direction
        dynamicITD: bool, optional
            Whether the ITD is applied separately. That requires preprocessed HRIR data
        dynamicILD: bool, optional
            Whether the ILD is computed and applied separately. At the moment this feature is not used (apart from applying the object gains)
        hrirInterpolation: bool, optional
            Whether the controller supports interpolation between neighbouring HRTF grid
            points. False means nearest neighbour (no interpolation), True
            enables barycentric interpolation.
        filterCrossfading: bool, optional
            Use a crossfading FIR filter matrix to avoid switching artifacts.
        fftImplementation: string, optional
            The FFT implementation to use. Default value enables VISR's default
            FFT library for the platform.
        headTrackingReceiver: class type, optional
            Class of the head tracking recveiver, None (default value) disables dynamic head tracking.
        headTrackingPositionalArguments: tuple optional
            Positional arguments passed to the constructor of the head tracking receiver object.
            Must be a tuple. If there is only a single argument, a trailing comma must be added.
        headTrackingKeywordArguments: dict, optional
            Keyword arguments passed to the constructor of the head tracking receiver. Must be a dictionary (dict)
        sceneReceiveUdpPort: int, optional
            A UDP port number where scene object metadata (in the S3A JSON format) is to be received).
            If not given (default), no network receiver is instantiated, and the object exposes a
            top-level parameter input port "objectVectorInput"
        """
        super( RealtimeDynamicHrirRendererAhrs, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )

        enableTracking = headTracking

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

        if sceneReceiveUdpPort is None:
            self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                         pml.DoubleBufferingProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
            self.parameterConnection( self.objectVectorInput,
                                     self.dynamicHrirRenderer.parameterPort("objectVector"))

        else:
            self.sceneReceiver = rcl.UdpReceiver( context, "SceneReceiver", self,
                                                 port = int(sceneReceiveUdpPort) )
            self.sceneDecoder = rcl.SceneDecoder( context, "SceneDecoder", self )
            self.parameterConnection( self.sceneReceiver.parameterPort("messageOutput"),
                                 self.sceneDecoder.parameterPort("datagramInput") )
            self.parameterConnection( self.sceneDecoder.parameterPort( "objectVectorOutput"),
                                 self.dynamicHrirRenderer.parameterPort("objectVector"))
        if enableTracking:
            self.trackingDevice = RazorAHRSWithUdpCalibrationTrigger(context, "HeadTrackingReceiver", self,
                                                *headTrackingPositionalArguments,
                                                **headTrackingKeywordArguments )
            self.parameterConnection( self.trackingDevice.parameterPort("orientation"), self.dynamicHrirRenderer.parameterPort("tracking"))

        self.audioConnection(  self.objectSignalInput, self.dynamicHrirRenderer.audioPort("audioIn"))
        self.audioConnection( self.dynamicHrirRenderer.audioPort("audioOut"), self.binauralOutput)
