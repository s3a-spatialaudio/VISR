#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Sep 12 10:28:06 2017

@author: gc1y17
"""

import visr
import pml
import numpy as np
import serial
from rotationFunctions import deg2rad

class serialReader(visr.AtomicComponent ):
    def __init__( self,
                  context, name, parent,
                  port,
                  baud
                  ):
        super( serialReader, self ).__init__( context, name, parent )
        self.yprVec =   np.zeros( 3, dtype = np.float32 )
        self.ser = serial.Serial(port, baud, timeout=0)   
        self.message = ""
        self.bufSize = 516
        self.sent = False
        self.trackingOutput = visr.ParameterOutput( "orientation", self, 
                                              pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
        self.trackingOutputProtocol = self.trackingOutput.protocolOutput()  
        
    def send_data(self,newdata):
        self.sent = True
        data = newdata
        data = data.replace("#","").replace("Y","").replace("P","").replace("R","").replace("=","").rstrip()
        try:
          yprvec = [float(i) for i in data.split(',')]
          if np.array(yprvec).size != 3:
            raise ValueError( 'yaw pitch roll bad format:'+str(np.array(yprvec)))
          ypr = self.trackingOutput.protocolOutput().data()
          ypr.orientation = [deg2rad(yprvec[0]),deg2rad(yprvec[1]),deg2rad(yprvec[2])]
#          print (yprvec)
          self.trackingOutput.protocolOutput().swapBuffers() 

        except ValueError:
          print ("Parsing went wrong because of a wrongly formatted string...")

        
    def parse_message (self, read, isLast): 
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
                          if isLast:
                              #print(" message sent: "+repr(read[ndlast+2:last+2]))#+" ( in mem "+repr(read[last:])+" ) over total "+(repr(read)))
                              self.send_data(read[ndlast+2:last+2])
                          self.message = read[last:]
                          #print(" message: "+repr(self.message))
                                  
    def process( self ):
#    while not connected:
#        #serin = ser.read()
#        connected = True
        self.sent = False
        totRead=0
        self.ser.read() #necessary for the .in_waiting to work

        while self.ser.isOpen and not(self.sent) :
            inBuffer = self.ser.in_waiting
           # print("inBuffer "+str(self.ser.read(self.bufSize)))
            if inBuffer > 0 :
              # print("\n ++ "+ str(inBuffer))  
               read = self.ser.read(self.bufSize).decode() #read the bytes and convert from binary array to ASCII
               totRead+=len(read)
               if totRead < inBuffer  :  
                 #  print(str(read))                    
                  # print(str(totRead)+" / "+ str(inBuffer))     
                   self.parse_message(read,False)
               else  :
               #    print("completed "+str(totRead)+" / "+ str(inBuffer))  
                   totRead=0
                   self.parse_message(read,True)             