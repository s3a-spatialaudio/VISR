from serial_reader import serialReader
from rotationFunctions import calcRotationMatrix
import visr
import numpy as np
import rrl

fs = 48000
bufferSize = 256

context = visr.SignalFlowContext( period=bufferSize, samplingFrequency=fs)

connected = False
port = "/dev/cu.usbserial-AJ03GSC8"
baud = 57600


controller = serialReader(context, "Controller", None,port, baud )

flow = rrl.AudioSignalFlow( controller )

orientation = flow.parameterSendPort("orientation")

numPos = 100

for blockIdx in range(0,numPos):
    print("rotation matrix #"+str(blockIdx)+":" )
    flow.process()
    
    if orientation.changed():
       print( calcRotationMatrix(np.array(orientation.data().orientation)))
       orientation.resetChanged()


#ser = serial.Serial('/dev/cu.usbserial-AJ03GSC8', 57600, timeout=0)   
#print(ser.readline())


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
   