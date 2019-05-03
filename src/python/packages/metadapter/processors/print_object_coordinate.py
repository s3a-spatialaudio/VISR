# Basic debug utility - print of object coordinates to screen

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

from copy import deepcopy
import math

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

class PrintPositionProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        self.objectId = int( arguments.attrib['objectID'] )
        self.active = strtobool( arguments.attrib['active'] )

    def processObjectVector( self, objectVector):
      
      objVectorNew = deepcopy(objectVector)
      for obj in objVectorNew:
          if int( obj['id']) == self.objectId:
            if self.active:
              obj_type = '%-5.5s' % obj['type']
            if obj_type == 'plane':
              rad = float(obj['direction']['refdist'])
              az = float(obj['direction']['az'])
              el = float(obj['direction']['el'])
              (x, y, z) = cart2sphDeg(rad,az,el)
              print ("\nx: {}, y: {}, z: {}".format(x, y, z))
              print ("r: {}, az: {}, el: {}".format(rad, az, el))
            elif obj_type == 'point':
              if 'x' in obj['position'].keys():
                x = float(obj['position']['x'])
                y = float(obj['position']['y'])
                z = float(obj['position']['z'])
                (rad, az, el) = cart2sphDeg(x,y,z)
              else:
                rad = float(obj['position']['radius'])
                az = float(obj['position']['az'])
                el = float(obj['position']['el'])
                (x, y, z) = cart2sphDeg(rad,az,el)
              print ("\nx: {}, y: {}, z: {}".format(x, y, z))
              print ("r: {}, az: {}, el: {}".format(rad, az, el))
      return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )
