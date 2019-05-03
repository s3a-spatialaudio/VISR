# Compress height component of all objects
# Note: won't affect channel based source types or reverberant part of point sources
# Richard Hughes, University of Salford, r.j.hughes@salford.ac.uk

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

# Required for the example class, but not for the template.
import numpy

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

class CompressHeightProcessor(SequenceProcessorInterface):
    
	def __init__(self, arguments ):
		SequenceProcessorInterface.__init__(self, arguments)
		self.active = strtobool( arguments.attrib['active'] )

	def processObjectVector( self, objectVector):
		objVectorNew = deepcopy(objectVector)    
		for obj in objectVector:
			obj_type = '%-5.5s' % obj['type']
			if self.active:
				if obj_type == 'plane':
					obj['direction']['el'] = 0.0
				elif obj_type == 'point':
					if 'x' in obj['position'].keys():
						(rad, az, elev) = cart2sphDeg(float(obj['position']['x']), float(obj['position']['y']), float(obj['position']['z']))
						(x, y, z) = sphDeg2cart(rad, az, 0.0)
						obj['position'] = { 'x': x, 'y': y, 'z': z }
					else:
						obj['position']['el'] = 0.0
					
		return objectVector

	def setParameter( self, key, valueList ):
		""" Set the parameter to a given value."""
		if (key == "active"):
			if len( valueList ) != 1:
				raise KeyError( "CompressHeightProcessor command \"active\" must contain 1 value." )
			self.active = bool( valueList[0] )



