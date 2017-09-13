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

class serialReader(visr.AtomicComponent ):
    def __init__( self,
                  port,
                  baud
                  ):
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
        #data = data.decode()
        self.sent = True
        data = newdata
        data = data.replace("#YPR=","").rstrip()
        data = data.split(',')
        print(data)        
        yprVec = self.trackingOutput.data()
        yprVec = data

        
    def parse_message (self, read, isLast): 
                 last = read.rfind("\n")
                 if last == -1:
                      self.message+=read
                      ##print(" no endl: "+repr(self.message))
                 else:     
                     ndlast = read.rfind("\n", 0, last)
                     if ndlast == -1:
                        
                         self.message+= read[:last+1]
                         ##print(" just one endl: "+repr(self.message))
                         if self.message.count('\n') >= 2:
                                 lastM = self.message.rfind("\n")
                                 ndlastM = self.message.rfind("\n", 0, lastM)
                                 #print(" message sent: "+repr(self.message[ndlastM+1:lastM+1]))
                                 self.send_data(self.message[ndlastM+1:lastM+1])
                                 self.message = read[last:]
                         else: 
                            self.message+= read[last+1:]
                     else:
                          if isLast:
                              #print(" message sent: "+repr(read[ndlast+1:last+1])+" ( in mem "+repr(read[last:])+" ) over total "+(repr(read)))
                              self.send_data(read[ndlast+1:last+1])
                          self.message = read[last:]
        
                          
    def process( self ):
#    while not connected:
#        #serin = ser.read()
#        connected = True
        print("ciao ++ ")  
        totRead=0
        self.ser.read() #necessary for the .in_waiting to work
        while self.ser.isOpen and not(self.sent) :
            inBuffer = self.ser.in_waiting
            if inBuffer > 0 :
               print("\n ++ "+ str(inBuffer))  
               read = self.ser.read(self.bufSize).decode() #read the bytes and convert from binary array to ASCII
               totRead+=len(read)
               if totRead < inBuffer  :  
                 #  print(str(read))                    
                   print(str(totRead)+" / "+ str(inBuffer))     
                   self.parse_message(read,False)
               else  :
                   print("completed "+str(totRead)+" / "+ str(inBuffer))  
                   totRead=0
                   self.parse_message(read,True)
             
             #print(repr(read)) #print the incoming string without putting a new-line ('\n') automatically after every print(

               #print(str(read))
             #print(str(sys.getsizeof(read)))
             
#           reading = self.ser.read.decode()
#           self.write_data(reading)
             

