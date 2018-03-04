# %BST_LICENCE_TEXT%

import visr
import pml
from rcl import UdpReceiver

from .razor_ahrs import RazorAHRS

class RazorAHRSWithUdpCalibrationTrigger( visr.CompositeComponent ):
    """ Receiver for the Razor AHRS head tracker with an additional UDP receiver port to zero the current look direction."""
    def __init__( self,
                  context, name, parent,
                  *,
                  calibrationPort,
                  ** razorArgs
                  ):
        super( RazorAHRSWithUdpCalibrationTrigger, self ).__init__( context, name, parent )
        self.trackingOutput = visr.ParameterOutput( "orientation", self,
                                              pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )

        razorArgs['calibrationInput'] = True # Reset the keyword argument (if already present)
        self.tracker = RazorAHRS( context, "Tracker", self, **razorArgs )
        self.triggerReceiver = UdpReceiver( context, "CalibrationTriggerReceiver", self,
                                           port = calibrationPort )
        self.parameterConnection( self.triggerReceiver.parameterPort("messageOutput"),
                                 self.tracker.parameterPort("calibration"))
        self.parameterConnection( self.tracker.parameterPort("orientation"),
                                 self.trackingOutput )
