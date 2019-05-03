# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

from copy import deepcopy
import math


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

def linInt(val, old_min, old_max, new_min, new_max):
    return (((val - old_min)*(new_max - new_min)) / (old_max - old_min)) + new_min

def azMapping(az,maxAz):
    # Limit azimuth values to +/- 180 deg
    azLim = (az + 180) % 360
    if azLim < 0:
        azLim = azLim + 360
    azLim = azLim - 180
    #print azLim

    
    if maxAz < 90:
        if azLim < -90:     # Reflect along y-axis
            azMod = abs(azLim) % -90
        elif azLim > 90:
            azMod = abs(azLim % -90)
        else:
            azMod = azLim
        azRemap = linInt(azMod, -90, 90, -maxAz, maxAz)
    else:
        azMod = az      #Do nothing
        azRemap = linInt(azMod, -180, 180, -maxAz, maxAz) 

    return azRemap

def GetCurrentGroup(obj):

    if 'groupname' in obj:
        current_group = obj['groupname']

    elif 'group' in obj:
        current_group = int(obj['group'])

    else:
        current_group = 0

    return current_group
	
class AzMappingProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        objStr = arguments.attrib['objectID']        
        self.objectId = [int(s) for s in objStr.split() if s.isdigit()]
        #print self.objectId
        self.active = strtobool( arguments.attrib['active'] )
        self.maxAz = float( arguments.attrib['maxAz'] )

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )

        objVectorNew = deepcopy(objectVector)
        for obj in objVectorNew:
            current_group = GetCurrentGroup(obj)
            
            #if int( obj['id']) in self.objectId:
            if current_group != 4:
                #print "Adapting object " + obj['id']
                #print current_group  
                #print int( obj['id'])
                if self.active:
                    if obj['type'] == 'plane':
                        az = float(obj['direction']['az'])
                        azRemap = azMapping(az, self.maxAz)
                        #print str(az) + " " + str(azRemap)
                        obj['direction']['az'] = azRemap
                    #if obj['type'] == 'pointreverb':
                    #   az = float(obj['position']['az'])
                    #   azRemap = azMapping(az, self.maxAz)
                    #   #print str(az) + " " + str(azRemap)
                    #   obj['position']['az'] = azRemap
                    else:
                        x = float(obj[ 'position' ]['x'])
                        y = float(obj[ 'position' ]['y'])
                        z = float(obj[ 'position' ]['z'])
                        (r,az,el) = cart2sphDeg(x,y,z)
                        azRemap = azMapping(az, self.maxAz)
                        #print str(az) + " " + str(azRemap)
                        (x,y,z) = sph2cartDeg(r,azRemap,el)
                        obj[ 'position' ] = { 'x': x, 'y': y, 'z': z }
                        # if int( obj['id']) == 2:
                        #     (r_temp,az_temp,el_temp) = cart2sphDeg(x,y,z)
                        #     print azRemap
                        #     print az_temp
        return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )
            print ("Active: {}".format(self.active) )

        if (key == "maxAz"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"maxAz\" must contain 1 value." )
            self.maxAz = float(valueList[0])
            print ("maxAz: {}".format(self.maxAz) )

        if (key == "objectID"):
            #Set new object list
            self.objectId = valueList

