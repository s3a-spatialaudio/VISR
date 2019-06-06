# Basic debug utility - print of object coordinates to screen

from metadapter import SequenceProcessorInterface

import math
import json
from time import gmtime

def rad2deg( w ):
	return 180.0/math.pi * w

def deg2rad( w ):
	return math.pi/180.0 * w

def cart2sphDeg(x,y,z):
	XsqPlusYsq = x**2 + y**2
	r = math.sqrt(XsqPlusYsq + z**2)               	# r
	elev = math.atan2(z,math.sqrt(XsqPlusYsq))     	# theta
	az = math.atan2(y,x)                           	# phi
	return r, rad2deg(az), rad2deg(elev)

def sphDeg2cart(r,az,elev):
	az = deg2rad(az)
	elev = deg2rad(elev)
	x = r*math.cos(az)*math.cos(elev)		   		# x
	y = r*math.sin(az)*math.cos(elev)		   		# y
	z = r*math.sin(elev)			   				# z
	return x, y, z

class saveObjectVector(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        
        # Set defaults
        
        # If processor is on
        if 'on' in arguments.attrib:
            self.on = bool(int(arguments.attrib['on']))
        else:
            self.on = True # Default is on
            
        # Verbose print out mode
        if "verbose" in arguments.attrib:
            self.verbose =  bool(int(arguments.attrib[ "verbose" ]))
        else:
            self.verbose = False
        
        # Update mode (overwrite or anything else)
        if "updateMode" in arguments.attrib:
            self.updateMode = arguments.attrib[ "updateMode" ]
        else:
            self.updateMode = 'overwrite'
        
        # Filename (and path) to write to file with
        if "writeToFile" in arguments.attrib:
            self.writeToFile =  arguments.attrib[ "writeToFile" ]
        else:
            self.writeToFile = 'TempObjVector'
        
    def processObjectVector( self, objectVector):
        
        if self.on == True:
            if len(self.writeToFile) > 0:
                filename = self.writeToFile
                if self.updateMode != 'overwrite':
                    t = gmtime()
                    # filename = ('%s_%04i%02i%02i%02i%02i%02i.txt' % (filename,t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec))
                    filename = ('%s_%04i%02i%02i%02i%02i.txt' % (filename,t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min))
                else:
                    filename = ('%s.txt' % (filename))
                with open(filename, 'w') as file:
                    if self.verbose:
                        print( '\nsaveObjectVector: %s\n' % filename )
                    file.write(json.dumps(objectVector))
        
        return objectVector

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if key == "updateMode":
            if valueList[0] == 0 or valueList[0] == 1:
                self.updateMode = int(valueList[0])
                if self.updateMode==0:
                    self.updateMode = 'update'
                else:
                    self.updateMode = 'overwrite'
            else:
                raise KeyError( "Command \"updateMode\" must be 0 or 1" )
        else:
            raise KeyError( "Unrecognised key in print object vector processor")