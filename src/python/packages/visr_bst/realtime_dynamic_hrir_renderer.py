# -*- coding: utf-8 -*-

# %BST_LICENCE_TEXT%

import visr
import pml

from .dynamic_hrir_renderer import DynamicBinauralRenderer

# Temporary, until tracking device is passed as a constructor parameter.
from .util.tracker.razor_ahrs import RazorAHRS


class RealtimeDynamicHrirRenderer(visr.CompositeComponent ):
    def __init__( self,
                 context, name, parent,
                 numberOfObjects,
                 port,
                 sofaFile,
                 enableSerial = True,
                 dynamicITD = True,
                 dynamicILD = True,
                 hrirInterpolation = True,
                 headTrackingCalibrationPort = None
                 ):
        super( RealtimeDynamicHrirRenderer, self ).__init__( context, name, parent )
        self.objectSignalInput = visr.AudioInputFloat( "audioIn", self, numberOfObjects )
        self.binauralOutput = visr.AudioOutputFloat( "audioOut", self, 2 )
        self.objectVectorInput = visr.ParameterInput( "objectVector", self, pml.ObjectVector.staticType,
                                                     pml.DoubleBufferingProtocol.staticType,
                                                     pml.EmptyParameterConfig() )

        self.dynamicBinauralRenderer = DynamicBinauralRenderer( context, "DynamicBinauralRenderer", self, numberOfObjects, sofaFile,
                                                                 headTracking = enableSerial,
                                                                 dynamicITD = dynamicITD,
                                                                 dynamicILD = dynamicILD,
                                                                 hrirInterpolation = hrirInterpolation
                                                               )
        if enableSerial:
            calibrationInputPresent = not headTrackingCalibrationPort is None
            self.serialReader = RazorAHRS(context, "HeadtrackingReceiver", self, port, yawOffset=90,rollOffset=-180, yawRightHand=True,
                                             calibrationInput = calibrationInputPresent)
            self.parameterConnection( self.serialReader.parameterPort("orientation"), self.dynamicBinauralRenderer.parameterPort("tracking"))

            if calibrationInputPresent:
                self.calibrationInput = visr.ParameterInput( "headTrackingCalibration",
                                                            self, pml.StringParameter.staticType,
                                                            pml.MessageQueueProtocol.staticType,
                                                            pml.EmptyParameterConfig() )
                self.parameterConnection( self.calibrationInput,
                                         self.serialReader.parameterPort("calibration"))

        self.parameterConnection( self.objectVectorInput, self.dynamicBinauralRenderer.parameterPort("objectVector"))
        self.audioConnection(  self.objectSignalInput, self.dynamicBinauralRenderer.audioPort("audioIn"))
        self.audioConnection( self.dynamicBinauralRenderer.audioPort("audioOut"), self.binauralOutput)
