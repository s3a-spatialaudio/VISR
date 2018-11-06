# -*- coding: utf-8 -*-
"""
Created on Fri Sept 2 2016

@author: JF
"""

import math

#from metadata_processor_interface import MetadataProcessorInterface
from metadapter import SequenceProcessorInterface

def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )
    
def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

# FUNCTION to convert radians to degrees
def rad2deg( w ):
    return 180.0/math.pi * w

# FUNCTION to convert degrees to radians
def deg2rad( w ):
    return w / 180.0*math.pi

# FUNCTION to convert cartesian to degrees
def cart2sphDeg(x,y,z):
    XsqPlusYsq = x**2 + y**2
    r = math.sqrt(XsqPlusYsq + z**2)               # r
    elev = math.atan2(z,math.sqrt(XsqPlusYsq))     # theta
    az = math.atan2(y,x)                           # phi
    return r, rad2deg(az), rad2deg(elev)

# FUNCTION to convert spherical degrees to cartesian
def sphDeg2cart(az,el,r):

    az = deg2rad(az)
    el = deg2rad(el)
    
    z = r * math.sin(el)
    rcoselev = r * math.cos(el)
    x = rcoselev * math.cos(az)
    y = rcoselev * math.sin(az)
    
    return x, y, z


#class SpreadProcessor(MetadataProcessorInterface):
class SpreadProcessor(SequenceProcessorInterface):
    
    def __init__(self, arguments ):
        #MetadataProcessorInterface.__init__(self, arguments)
        SequenceProcessorInterface.__init__(self, arguments)        
        
        self.objectLookup = {}
        self.spread = {}
        
        for groupNode in arguments.iter( 'group' ):
            groupName = groupNode.attrib['name']
            objsStr = groupNode.attrib['objectIds']
            initialSpread = float(groupNode.attrib['initialSpread'])
            objIds = [int(s) for s in objsStr.split() if s.isdigit()]
            for objId in objIds:
                self.objectLookup[objId] = groupName
            self.spread[groupName] = initialSpread

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        for obj in objectVector:
            id = int( obj['id'] )
            if id in self.objectLookup:
                group = self.objectLookup[id]
                
                ### Get the original position and convert to spherical
                object_type = obj['type']
                
                if object_type == 'point':
                    if 'x' in obj['position']:
                        
                        # Get the angle in degrees
                        x = float(obj['position']['x'])
                        y = float(obj['position']['y'])
                        z = float(obj['position']['z'])
                         
                        orig_dist, orig_az, orig_el = cart2sphDeg(x,y,z)
                        
                    elif 'az' in obj['position']:
                        orig_az = float(obj['position']['az'])
                        orig_el = float(obj['position']['el'])
                        orig_dist = float(obj['position']['radius'])
                
                elif object_type == 'plane':
                    orig_az = obj['direction']['az']		# Get original positions
                    orig_el = obj['direction']['el']		# ...
                    orig_dist = obj['direction']['refdist']	# ...
                     

                adjustSpread = self.spread[group]		# Get the spread value
                
                orig_az = orig_az % 360					# Convert the azimuth to the range 0 to 360
                orig_el = orig_el % 360					# Convert the elevation to the range 0 to 360
                
                if orig_az > 180:						# Convert azimuth greater than 180 to negative, so the spread multiplier works
                    orig_az = orig_az - 360				# ...
                if orig_el > 180:						# ... same for elevation
                    orig_el = orig_el - 360				# ...
                
                az = orig_az * adjustSpread 			# New position = original position * spread
                el = orig_el * adjustSpread				# New position = original position * spread
                
                
                if object_type == 'point':
                    if 'x' in obj['position']:
                        # Convert back to cartesian
                        newx, newy, newz = sphDeg2cart(az,el,orig_dist)
                        # Set the metadata
                        obj['position'] = { 'x': newx, 'y': newy, 'z': newz }
     
                    elif 'az' in obj['position']:
                        obj['position'] = { 'az': az, 'el': el, 'radius': orig_dist }
 
                      
                elif object_type == 'plane':
                    obj['direction'] = { 'az': az, 'el': el, 'refdist': orig_dist }
                
                
        return objectVector
        
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
            raise KeyError( "SpreadProcessor supports only commands consisting of a single float value" )
        if not key in self.spread:
            raise KeyError( "SpreadProcessor: The command name does not correspond to a known group." )
        spread_in = valueList[0]
        self.spread[key] = spread_in