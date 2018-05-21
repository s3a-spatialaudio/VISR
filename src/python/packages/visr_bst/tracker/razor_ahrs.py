# -*- coding: utf-8 -*-

# Copyright (C) 2017-2018 Andreas Franck and Giacomo Costantini
# Copyright (C) 2017-2018 University of Southampton

# VISR Binaural Synthesis Toolkit (BST)
# Authors: Andreas Franck and Giacomo Costantini
# Project page: http://cvssp.org/data/s3a/public/BinauralSynthesisToolkit/


# The Binaural Synthesis Toolkit is provided under the ISC (Internet Systems Consortium) license
# https://www.isc.org/downloads/software-support-policy/isc-license/ :

# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
# OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


# We kindly ask to acknowledge the use of this software in publications or software.
# Paper citation:
# Andreas Franck, Giacomo Costantini, Chris Pike, and Filippo Maria Fazi,
# “An Open Realtime Binaural Synthesis Toolkit for Audio Research,” in Proc. Audio Eng.
# Soc. 144th Conv., Milano, Italy, 2018, Engineering Brief.
# http://www.aes.org/e-lib/browse.cfm?elib=19525

# The Binaural Synthesis Toolkit is based on the VISR framework. Information about the VISR,
# including download, setup and usage instructions, can be found on the VISR project page
# http://cvssp.org/data/s3a/public/VISR .

import visr
import pml

import numpy as np
import serial

from ..util.rotation_functions import deg2rad

class RazorAHRS(visr.AtomicComponent ):
    """
    Component to receive tracking data from a Razor AHRS device through a serial port.
    """
    def __init__( self,
                  context, name, parent,
                  port,
                  yawOffset=0,
                  pitchOffset=0,
                  rollOffset=0,
                  yawRightHand=False,
                  pitchRightHand=False,
                  rollRightHand=False,
                  calibrationInput = False # Whether to instantiate an input port to set the orientation.
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
        yawOffset:
            Initial offset for the yaw component, default 0.0
        pitchOffset :  float
            Offset for the pitch value, in degree
        rollOffset : float:
            Initial value for the roll component, default 0.0
        yawRightHand: bool
            Whehther the yaw coordinate is interpreted as right-hand
            (mathematically negative) rotation. Default: False
        pitchRightHand: bool
            Whehther the pitch coordinate is interpreted as right-hand
            (mathematically negative) rotation. Default: False
        rollRightHand: bool
            Whehther the roll coordinate is interpreted as right-hand
            (mathematically negative) rotation. Default: False
        calibrationInput: bool
            Flag to determine whehter the component has an additional input "calibration"
            that resets the orientation offsets. At the moment, this input is of
            type StringParameter, and the value is ignored.

        TODO: Check whether to support ListenerPosition objects as calibration triggers
        to set the orientation to an arbitrary value
        """


        super( RazorAHRS, self ).__init__( context, name, parent )
        self.yprVec =   np.zeros( 3, dtype = np.float32 )
        baudRate = 57600
        self.ser = serial.Serial(port, baudRate, timeout=0)
        self.message = ""
        self.sent = False
        self.trackingOutput = visr.ParameterOutput( "orientation", self,
                                              pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
        self.trackingOutputProtocol = self.trackingOutput.protocolOutput()

        if calibrationInput:
            self.calibrationInput = visr.ParameterInput( "calibration", self,
                                                         pml.StringParameter.staticType,
                                                         pml.MessageQueueProtocol.staticType,
                                                         pml.EmptyParameterConfig() )
        else:
            self.calibrationInput = None

        self.sentN = 0
        self.parsedN = 0
        self.ser.read() #necessary for the .in_waiting to work
        self.procN =0
        self.yawOffset = yawOffset
        self.pitchOffset = pitchOffset
        self.rollOffset = rollOffset
        self.yawRightHand = yawRightHand
        self.pitchRightHand = pitchRightHand
        self.rollRightHand= rollRightHand
        self.orientation = np.array( [0.0, 0.0, 0.0 ] ) # Current orientation, unadjusted, in radian

    def send_data(self,newdata):
        data = newdata
        data = data.replace("#","").replace("Y","").replace("P","").replace("R","").replace("=","").rstrip()
        try:
          yprvec = [float(i) for i in data.split(',')]
          if self.yawRightHand:
#              print(ypr.orientation[0])
              yprvec[0]*= -1
#              print(ypr.orientation[0])
          if self.pitchRightHand:
              yprvec[1]*= -1
          if self.rollRightHand:
              yprvec[2]*= -1

          if np.array(yprvec).size != 3:
            raise ValueError( 'yaw pitch roll bad format:'+str(np.array(yprvec)))

          self.orientation = yprvec # Store the current position

          ypr = self.trackingOutput.protocolOutput().data()


          # [deg2rad(yprvec[0]+self.yawOffset),deg2rad(yprvec[1]+self.pitchOffset),deg2rad(yprvec[2]+self.rollOffset)]

          ypr.orientation = [deg2rad(self.orientation[0] + self.yawOffset),
                             deg2rad(self.orientation[1] + self.pitchOffset),
                             deg2rad(self.orientation[2] + self.rollOffset)]

          self.sentN = self.sentN+1
#          print("%d serial parsing %f sec"%(self.procN,time.time()-self.start))
#          print("[%d,%d,%d]"%(yprvec[0]+self.yawOffset,yprvec[1]+self.pitchOffset,yprvec[2]+self.rollOffset))
          self.trackingOutput.protocolOutput().swapBuffers()

        except ValueError:
          print ("Parsing went wrong because of a wrongly formatted string...")


    def parse_message (self, read):
         last = read.rfind("\r\n")
         if last == -1:
              self.message+=read
              #print(" no endl: "+repr(self.message))
         else:
             ndlast = read.rfind("\r\n", 0, last)
             if ndlast == -1:
                 #print(" just one endl bef: "+repr(self.message))
                 self.message+= read[:last+2]
                 #print(" just one endl: "+repr(self.message))
                 if self.message.count('\r\n') >= 2:
                         lastM = self.message.rfind("\r\n")
                         ndlastM = self.message.rfind("\r\n", 0, lastM)
                         lastN = self.message[ndlastM+2:lastM].rfind("\n")
                         lastR = self.message[ndlastM+2:lastM].rfind("\r")
                         if lastN != -1:
                             ndlastM = lastN
                         if lastR != -1:
                             ndlastM = lastR
                         #print(" message sent: "+repr(self.message[ndlastM+2:lastM+2]))
                         self.send_data(self.message[ndlastM+2:lastM+2])
                         self.message = read[last:]
                 else:
                    self.message+= read[last+2:]
                    #print(" not sent: "+repr(self.message))
             else:
                  #print(" message sent: "+repr(read[ndlast+2:last+2]))#+" ( in mem "+repr(read[last:])+" ) over total "+(repr(read)))
                  self.send_data(read[ndlast+2:last+2])
                  self.sent = True
                  self.message = read[last:]
         self.parsedN = self.parsedN+1

    def process( self ):
        if not self.calibrationInput is None:
            calInputProtocol = self.calibrationInput.protocolInput()
            if not calInputProtocol.empty():
                # We are not interested in the content, but just use it as a trigger to use the
                # most recent orientation to set the compensation values.
                self.yawOffset = - self.orientation[0]
                self.pitchOffset = - self.orientation[1]
                self.rollOffset = - self.orientation[2]
                calInputProtocol.clear()

        self.procN=self.procN+1
        inBuffer = self.ser.in_waiting
        if inBuffer == 0 :
            return
        else :
            try:
                read = self.ser.read(inBuffer).decode('iso-8859-1') #read the bytes and convert from binary array to ASCII
                totRead=len(read)
                if totRead == 0 :
                    return
                else  :
                    self.parse_message(read)
            except BaseException as ex:
                print( "Error while decoding tracking data message: %s" % str(ex) )
