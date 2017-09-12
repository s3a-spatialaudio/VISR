import threading
import math
import serial
import numpy as np

connected = False
port = '/dev/cu.usbserial-AJ03GSC8'
baud = 57600
idMatrix = np.identity(3)

def calcRotationMatrix(ypr):  
  if ypr.size == 3 :
   # print(type(ypr))
    psi = np.float32(ypr.item(0))
   # print(type(psi))
    the = np.float32(ypr.item(1))
   # print(type(the))
    phi = np.float32(ypr.item(2))


    a11 = math.cos(the) * math.cos(phi)
    a12 = np.math.cos(the) * np.math.sin(phi)
    a13 = -np.math.sin(the)
    
    a21 = np.math.sin(psi) * np.math.sin(the) * np.math.cos(phi) - np.math.cos(psi) * np.math.sin(phi)
    a22 = np.math.sin(psi) * np.math.sin(the) * np.math.sin(phi) + np.math.cos(psi) * np.math.cos(phi)
    a23 = np.math.cos(the) * np.math.sin(psi)

    a31 = np.math.cos(psi) * np.math.sin(the) * np.math.cos(phi) + np.math.sin(psi) * np.math.sin(phi)
    a32 = np.math.cos(psi) * np.math.sin(the) * np.math.sin(phi) - np.math.sin(psi) * np.math.cos(phi)
    a33 = np.math.cos(the) * np.math.cos(psi)
    
    rotation = np.matrix([[a11, a12, a13], [a21, a22, a23], [a31, a32, a33]])
    return rotation
  else:
    return idMatrix
      
class serialReader():
    def __init__( self,
                  port,
                  baud
                  ):
        self.yprVec =   np.zeros( 3, dtype = np.float32 )
        self.ser = serial.Serial(port, baud, timeout=0)   
        #self.message = "\n"
        self.message = ""
        self.idMatrix = np.identity(3)

    def calcRotationMatrix(self): 
        ypr = np.array(self.yprVec)
        if ypr.size == 3 :
            # print(type(ypr))
            psi = np.float32(ypr.item(0))
            # print(type(psi))
            the = np.float32(ypr.item(1))
            # print(type(the))
            phi = np.float32(ypr.item(2))


            a11 = math.cos(the) * math.cos(phi)
            a12 = np.math.cos(the) * np.math.sin(phi)
            a13 = -np.math.sin(the)
            
            a21 = np.math.sin(psi) * np.math.sin(the) * np.math.cos(phi) - np.math.cos(psi) * np.math.sin(phi)
            a22 = np.math.sin(psi) * np.math.sin(the) * np.math.sin(phi) + np.math.cos(psi) * np.math.cos(phi)
            a23 = np.math.cos(the) * np.math.sin(psi)
            
            a31 = np.math.cos(psi) * np.math.sin(the) * np.math.cos(phi) + np.math.sin(psi) * np.math.sin(phi)
            a32 = np.math.cos(psi) * np.math.sin(the) * np.math.sin(phi) - np.math.sin(psi) * np.math.cos(phi)
            a33 = np.math.cos(the) * np.math.cos(psi)
    
            rotation = np.matrix([[a11, a12, a13], [a21, a22, a23], [a31, a32, a33]])
            return rotation
        else:
            return self.idMatrix
   
    def get_data(self):        
        return self.yprVec
        
    def send_data(self,data):
        #data = data.decode()
        data = data.replace("#YPR=","").rstrip()
        data = data.split(',')
        self.yprVec = data
        print(self.calcRotationMatrix())
        
    def parse_message (self, read): 


                 last = read.rfind("\n")
                 if last == -1:
                      self.message+=read
                      ##print(" no endl: "+repr(self.message))
                 else:     
                     ndlast = read.rfind("\n", 0, last)
                     if ndlast == -1:
                        
                         self.message+= read[:last+1]
                         ##print(" just one endl: "+repr(self.message))
                         if self.message.count('\n') == 2:
                                 lastM = self.message.rfind("\n")
                                 ndlastM = self.message.rfind("\n", 0, lastM)
                                 #print(" message completed: "+repr(self.message))
                                 self.send_data(self.message[ndlastM+1:lastM+1])
                                 self.message = read[last:]
                         else: 
                            self.message+= read[last+1:]
                     else:
                          #print("2endl message completed: "+repr(read))
                          self.send_data(read[ndlast+1:last+1])
                          self.message = read[last:]
        
                          
    def read_from_port(self):
#    while not connected:
#        #serin = ser.read()
#        connected = True
        while self.ser.isOpen:
           #if (self.ser.inWaiting()>0): #if incoming bytes are waiting to be read from the serial input buffer
             read = self.ser.read(26).decode() #read the bytes and convert from binary array to ASCII
             #print(repr(read)) #print the incoming string without putting a new-line ('\n') automatically after every print(
             self.parse_message(read)
             
                
#           reading = self.ser.read.decode()
#           self.write_data(reading)
             

 
controller = serialReader(port,baud)
controller.read_from_port()






"""OFFLINE TEST
controller = serialReader(port,baud)
numPos = 20
usb = serial.Serial(port, baud) 
for blockIdx in range(0,numPos):
   read = usb.read(26).decode() #read the bytes and convert from binary array to ASCII
   print(repr(read)) 
   controller.parse_message(read)
   print()
  
"""
   
   
##controller.send_data(usb.readline())
  # print(controller.yprVec)
#print(usb.readline())
#controller.parse_message(stri)


#print(controller.get_data())
#print(calcRotationMatrix(np.array(controller.get_data())))


#thread = threading.Thread(target=read_from_port, args=(serial_port,))
#thread.daemon = True
#thread.start()
#try:
#   while True:
#     thread.join(1)
#except KeyboardInterrupt:
#   print ("^C is caught, exiting")
   