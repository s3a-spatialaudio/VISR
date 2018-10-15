# -*- coding: utf-8 -*-
"""
Created on Fri Oct 06 2017

@author: Jon Francombe, James Woodcock, Rick Hughes
"""

import numpy
import math
from sys import version_info

if version_info.major <= 2:
    import OSC
else:
    # Use the self-made port for Python 3 (experimental)
    from ..thirdparty import OSC as OSC


#from metadata_processor_interface import MetadataProcessorInterface
# Metadapter2 version:
from metadapter import SequenceProcessorInterface


# FUNCTION to convert radians to degrees
def rad2deg( w ):
    return 180.0/math.pi * w

# FUNCTION to convert degrees to radians
def deg2rad( w ):
    return w / 180.0*math.pi

# FUNCTION to convert spherical degrees to cartesian
def sphDeg2cart(az,el,r):

    az = deg2rad(az)
    el = deg2rad(el)

    z = r * math.sin(el)
    rcoselev = r * math.cos(el)
    x = rcoselev * math.cos(az)
    y = rcoselev * math.sin(az)

    return x, y, z

# FUNCTION to convert cartesian to degrees
def cart2sphDeg(x,y,z):
    XsqPlusYsq = x**2 + y**2
    r = math.sqrt(XsqPlusYsq + z**2)               # r
    elev = math.atan2(z,math.sqrt(XsqPlusYsq))     # theta
    az = math.atan2(y,x)                           # phi
    return r, rad2deg(az), rad2deg(elev)

# FUNCTIONS to convert between dB and linear
def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )

def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )



class MDOPhonesProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        if 'on' in arguments.attrib:
            self.on = int(arguments.attrib['on'])
        else:
            self.on = 1 # Default is on

        # Set up a list of which MDO speakers are available (hard coded 100 max)
        self.idlist = [0] * 100

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        if self.on:

            # Determine which MDO channels are to be used
			available = [0] * 4										# Hard coded at max 4 (modulo after this)
			for i in range(len(self.idlist)):
				available[i%4] = available[i%4]+self.idlist[i]

			for i,obj in enumerate(objectVector):

				target_mdo_channel = int(obj['mdo_channel'])

				if target_mdo_channel != 0 and available[target_mdo_channel-1]:            # If there's a loudspeaker available at the specified mdo channel
					obj['type'] = 'channel'
					obj['outputChannels'] = target_mdo_channel + 2  # MDO channel number plus 2 (the stereo speakers) - this could be coded more flexibly

        # Note: this has to be returned otherwise no objects are replayed
        #print objectVector
        return objectVector



    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""

        if (key == "toggle"):
            if (len( valueList ) == 2):
				id = int(valueList[0])
				toggle = int(valueList[1])
				print 'MDOPhones processor id %i toggle %i' % (id, toggle)
				if toggle == 0:
					self.idlist[id] = 0
				else:
					self.idlist[id] = 1
            else:
            	raise KeyError( "Number of arguments must be 2" )
		if (key == "reset"):
			print 'MDOPhones processor reset'
			self.idlist = [0] * len(self.idlist)
		else:
			raise KeyError( "MDOPhonesProcessor: key input not recognised")

