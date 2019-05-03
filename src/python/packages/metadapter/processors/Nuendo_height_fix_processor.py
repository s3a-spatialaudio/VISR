# Apply fix to Nuendo height data present in first radio drama mix
# Not necessary in more up-to-date ADM files or Nuendo sessions
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

def remap(val, old_min, old_max, new_min, new_max):
	val = (((val - old_min)*(new_max - new_min)) / (old_max - old_min)) + new_min
	return val

class NuendoHeightFixProcessor(SequenceProcessorInterface):
    
	def __init__(self, arguments ):
		SequenceProcessorInterface.__init__(self, arguments)
		self.active = strtobool( arguments.attrib['active'] )

	def processObjectVector( self, objectVector):
		objVectorNew = deepcopy(objectVector)    
		for obj in objectVector:
			if self.active:

				# Get length 5 object type
				obj_type = '%-5.5s' % obj['type']

				# Get x, y, z
				if obj_type == 'plane':
					rad = float(obj['direction']['refdist'])
					az = float(obj['direction']['az'])
					el = float(obj['direction']['el'])
					(x, y, z) = cart2sphDeg(rad,az,el)
				elif obj_type == 'point':
					if 'x' in obj['position'].keys():
						x = float(obj['position']['x'])
						y = float(obj['position']['y'])
						z = float(obj['position']['z'])
					else:
						rad = float(obj['position']['radius'])
						az = float(obj['position']['az'])
						el = float(obj['position']['el'])
						(x, y, z) = cart2sphDeg(rad,az,el)

				# Remap
				if z >= 1.0:
					z = remap(z, 1.0, 2.0, 0.0, 2.0)
				else:
					z = remap(z, -2.0, 1.0, -2.0, 0.0)

				# Update object
				if obj_type == 'plane':
					(rad, az, elev) = cart2sphDeg(x, y, z)
					obj['direction'] = { 'az': az, 'el': el, 'refdist': rad }
				elif obj_type == 'point':
					if 'x' in obj['position'].keys():
						obj['position'] = { 'x': x, 'y': y, 'z': z }
					else:
						(rad, az, elev) = cart2sphDeg(x, y, z)
						obj['position'] = { 'az': az, 'el': el, 'radius': rad }

		return objectVector

	def setParameter( self, key, valueList ):
		""" Set the parameter to a given value."""
		if (key == "active"):
			if len( valueList ) != 1:
				raise KeyError( "CompressHeightProcessor command \"active\" must contain 1 value." )
			self.active = bool( valueList[0] )



