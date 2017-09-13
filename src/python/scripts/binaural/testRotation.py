from serial_reader import serialReader
import math
import numpy as np
import rrl

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
 
controller = serialReader(port,baud)
print("ciao + ")  
flow = rrl.AudioSignalFlow( controller )

orientation = flow.parameterSendPort("orientation")

numPos = 72

for blockIdx in range(0,numPos):
    flow.process()
    
    if orientation.changed():
        print( "Updated hrir gains: %s." % str(np.array(orientation.data()) ))
        orientation.resetChanged()






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
   