# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

from copy import deepcopy
import math
import json


def cart2sphDeg(x,y,z):
    XsqPlusYsq = x**2 + y**2
    r = math.sqrt(XsqPlusYsq + z**2)               # r
    elev = math.atan2(z,math.sqrt(XsqPlusYsq))     # theta
    az = math.atan2(y,x)                           # phi
    return r, rad2deg(az), rad2deg(elev)

def sph2cartDeg(r,az,elev):
    azRad = deg2rad(az)
    elevRad = deg2rad(elev)
    x = r * math.cos(azRad) * math.cos(elevRad)
    y = r * math.sin(azRad) * math.cos(elevRad)
    z = r * math.sin(elevRad)
    return x, y, z

def deg2rad(deg):
    rad = (math.pi/180)*deg
    return rad

def rad2deg(rad):
    deg = 180/math.pi * rad
    return deg

def linInterp(val, old_min, old_max, new_min, new_max):
    '''Linear interpolation'''

    return (((val - old_min)*(new_max - new_min)) / (old_max - old_min)) + new_min

def azRemap(az, newMin, newMax):
    """Rescale data in range [newMin newMax]""" 
    
    if az < newMin:
        azRemapped = linInterp(az, newMin, -180, newMin, 0)
    elif az > newMax:
        azRemapped = linInterp(az, newMax, 180, newMax, 0)
    else:
        azRemapped = az

    return azRemapped
    
def azLimit(az):
    """Limits azimuth to +/- 180"""

    azLim = (az + 180) % 360
    if azLim < 0:
        azLim = azLim + 360
    return azLim - 180
    

def GetGroupName(obj):
    """Gets the current object category, returns zero if no category is found"""

    if 'group_name' in obj:
        group_name = obj['group_name']
    else:
        group_name = 'none'
    
    return group_name
    
class SemanticStereoProcessor(SequenceProcessorInterface):
    
    def __init__(self, arguments ):

        SequenceProcessorInterface.__init__(self, arguments)

        # Set default values        
        if 'maxAz' in arguments.attrib:
            self.maxAz = float( arguments.attrib['maxAz'] )
        else:
            self.maxAz = 30.0   #Default is 30

        if 'minAz' in arguments.attrib:
            self.minAz = float( arguments.attrib['minAz'] )
        else:
            self.minAz = -30.0   #Default is -30
        
        if 'positionActive' in arguments.attrib:
            self.positionActive = bool(int(arguments.attrib['positionActive']))
        else:
            self.positionActive = True # Default is on  
            
        if 'levelActive' in arguments.attrib:
            self.levelActive = bool(int(arguments.attrib['levelActive']))
        else:
            self.levelActive = True # Default is on 

        if 'positionCats' in arguments.attrib:
            cat = arguments.attrib['positionCats']
            self.positionCats = cat.split()
        else:
            self.positionCats = 'none'
        
        if 'levelAdj' in arguments.attrib:
           
            levelAdjData = arguments.attrib['levelAdj']
            levelAdjData = levelAdjData.replace( '\'', '\"' )  # Fix JSON string
            levelAdjData = json.loads(levelAdjData)
            # Force keys to be lower case
            dict((k.lower(), v) for k,v in levelAdjData.items())
            self.levelAdj = levelAdjData

        else:
            self.levelAdj = 'none'   

        if 'verbose' in arguments.attrib:
            self.verbose = bool(int(arguments.attrib['verbose']))
        else:
            self.verbose = False  # Default is False

    def processObjectVector( self, objectVector):
        """Main processing function"""
        
        objVectorNew = deepcopy(objectVector)  # Make a copy of the object vector
        
        for obj in objVectorNew:    # Iterate over objects
            
            groupName = GetGroupName(obj)
            
            if self.positionActive: # Check if position rempapping is on
                
                # Only operate on objects from specified categories
                if groupName.lower() in self.positionCats or groupName.lower() == 'none':    
                    
                    if 'direction' in obj:
                        
                        az = float(obj['direction']['az'])
                        azRemapped = azRemap(azLimit(az), self.minAz, self.maxAz)
                        obj['direction']['el'] = 0.0    # Squash height 
                        obj['direction']['az'] = azRemapped
                    
                    elif 'position' in obj and 'az' in obj['position']:
                    
                        az = float(obj['direction']['az'])
                        azRemapped = azRemap(azLimit(az), self.minAz, self.maxAz)
                        obj['direction']['el'] = 0.0    # Squash height
                        obj['direction']['az'] = azRemapped
                    
                    elif 'position' in obj and 'x' in obj['position']:
                    
                        x = float(obj[ 'position' ]['x'])
                        y = float(obj[ 'position' ]['y'])
                        z = float(obj[ 'position' ]['z'])
                        
                        (r,az,el) = cart2sphDeg(x,y,z)
                        
                        azRemapped = azRemap(azLimit(az), self.minAz, self.maxAz)
                        
                        (x,y,z) = sph2cartDeg(r,azRemapped,0.0)
                        obj[ 'position' ] = { 'x': x, 'y': y, 'z': z }
                    
                    if self.verbose:
                        print("Original position = %d\n" % az)
                        print("Remapped position = %d\n" % azRemapped)
                else:
                    if self.verbose:
                        print("Original position retained")

            if self.levelActive: #Check if level rempapping is on

                if groupName.lower() in self.levelAdj:
                                       
                    dbAdj = self.levelAdj[groupName.lower()]
                    obj['level'] = float(obj['level']) * math.pow(10,dbAdj/20)
                    if self.verbose:
                        print("Applied a %f dB level adjustment to Object %d\n" % (dbAdj, int(obj['id'])))
        
        return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""

        if (key == "positionActive"):
            if len( valueList ) != 1:
                raise KeyError( "StereoDownmixProcessor command \"positionActive\" must contain 1 value." )
            self.positionActive = bool( valueList[0] )
            print ("positionActive: {}".format(self.positionActive) )

        if (key == "levelActive"):
            if len( valueList ) != 1:
                raise KeyError( "StereoDownmixProcessor command \"levelActive\" must contain 1 value." )
            self.levelActive = bool( valueList[0] )
            print ("levelActive: {}".format(self.levelActive) )

        if (key == "maxAz"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"maxAz\" must contain 1 value." )
            self.maxAz = float(valueList[0])
            print ("maxAz: {}".format(self.maxAz) )


