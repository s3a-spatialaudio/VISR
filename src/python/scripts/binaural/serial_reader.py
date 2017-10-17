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
import time

class serialReader(visr.AtomicComponent ):
    def __init__( self,
                  context, name, parent,
                  port,
                  baud,
                  yawOffset=0,
                  pitchOffset=0,
                  rollOffset=0,
                  yawRightHand=False,
                  pitchRightHand=False,
                  rollRightHand=False
                  ):
        super( serialReader, self ).__init__( context, name, parent )
        self.yprVec =   np.zeros( 3, dtype = np.float32 )
        self.ser = serial.Serial(port, baud, timeout=0)   
        self.message = ""
        self.sent = False
        self.trackingOutput = visr.ParameterOutput( "orientation", self, 
                                              pml.ListenerPosition.staticType,
                                              pml.DoubleBufferingProtocol.staticType,
                                              pml.EmptyParameterConfig() )
        self.trackingOutputProtocol = self.trackingOutput.protocolOutput()  
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

#          yprvec = [float(i) for i in data]
#          print(yprvec)
#          yprvec = data
          if np.array(yprvec).size != 3:
            raise ValueError( 'yaw pitch roll bad format:'+str(np.array(yprvec)))
          ypr = self.trackingOutput.protocolOutput().data()
          ypr.orientation = [deg2rad(yprvec[0]+self.yawOffset),deg2rad(yprvec[1]+self.pitchOffset),deg2rad(yprvec[2]+self.rollOffset)]
          
        
          self.sentN = self.sentN+1
#          print("%d serial parsing %f sec"%(self.procN,time.time()-self.start)) 
#          print("[%d,%d,%d]"%(yprvec[0]+self.yawOffset,yprvec[1]+self.pitchOffset,yprvec[2]+self.rollOffset))
          self.trackingOutput.protocolOutput().swapBuffers() 
          
        except ValueError:
          print ("Parsing went wrong because of a wrongly formatted string...")

        
    def parse_message (self, read): 
#       try:
#
#                 self.message+=read.rstrip()
##                 print(self.message) 
##                 print("message " + self.message)
#                 
##                for m in re.finditer(r"\r\n(\S+)\r\n", strz1,overlapped=True):
##                    print('%02d-%02d: %s' % (m.start(), m.end(), m.group(0)))
##                        
#                 lastline = re.findall('#YPR=(\S+,\S+,\S+)', self.message)
#                 
##                 for lastline in re.findall("#YPR=(\d+[.]\d+),(\d+[.]\d+),(\d+[.]\d+)", self.message):
##                     pass
#                 values = lastline[-1]
##                 print(values)                    
#                 self.message=''
##                 values = lastline.group(0)
###                 re.findall("^#YPR=(\d+[.]\d+),(\d+[.]\d+),(\d+[.]\d+)", lastline)
###                 lastline = (self.message).splitlines()[-1].rstrip()
###                 print("2nd "+ndlastline)
###                 print("last "+lastline)
###                 values = re.findall("^#YPR=(\d+[.]\d+),(\d+[.]\d+),(\d+[.]\d+)", lastline)
###                 print(values)
#                 if values :
#                         print(values)
##                         self.send_data(values)
##                         self.message = ''
#
#
#
#                         
##                 else : 
##                         ndlastline = (self.message+read).splitlines()[-2].rstrip()
##                         values = re.findall("^#YPR=(\d+[.]\d+),(\d+[.]\d+),(\d+[.]\d+)", ndlastline)
####                     print(len(values))
###                     
##                         if values :
##                             print("2nd")
##                             print(values)
###                         self.send_data(values)
##                             self.message = lastline
#                         
#       except ValueError:
#          print ("Parsing went wrong...")                  
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
#                 print(" message: "+repr(self.message))
                                  
    def process( self ):
#    while not connected:
#        #serin = ser.read()
#        connected = True
        self.start = time.time()
#        print("%d serial parsing %f sec"%(self.procN,time.time()-self.start)) 
#        self.sent = False
#        totRead=0
        self.procN=self.procN+1        
#        while self.ser.isOpen and not self.sent :    
        inBuffer = self.ser.in_waiting
        if inBuffer == 0 :
            return
#            print("++ "+ str(inBuffer))  
        else :
#           print("%d serial parsing %f sec"%(self.procN,time.time()-self.start)) 
           read = self.ser.read(inBuffer).decode() #read the bytes and convert from binary array to ASCII
           totRead=len(read)           
#           print(str(totRead)+" / "+ str(inBuffer))  
           if totRead == 0 :
               return
           else  :
               self.parse_message(read)             
           #    print("completed "+str(totRead)+" / "+ str(inBuffer))  
        