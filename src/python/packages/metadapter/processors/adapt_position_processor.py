# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

from copy import deepcopy
import math

def rad2deg( w ):
    return 180.0/math.pi * w

def cart2sphDeg(x,y,z):
    XsqPlusYsq = x**2 + y**2
    r = math.sqrt(XsqPlusYsq + z**2)               # r
    elev = math.atan2(z,math.sqrt(XsqPlusYsq))     # theta
    az = math.atan2(y,x)                           # phi
    return r, rad2deg(az), rad2deg(elev)

class AdaptPositionProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        self.objectId = int( arguments.attrib['objectID'] )
        self.active = strtobool( arguments.attrib['active'] )
        self.x = float( arguments.attrib['x'] )
        self.y = float( arguments.attrib['y'] )
        self.z = float( arguments.attrib['height'] )

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )

        objVectorNew = deepcopy(objectVector)
        for obj in objVectorNew:
            if int( obj['id']) == self.objectId:
                if self.active:
                    if obj['type'] == 'plane':
                        (rad, az, el ) = cart2sphDeg( self.x, self.y, self.z )
                        obj['direction'] = { 'az': az, 'el': el, 'refdist': rad }
                    else:
                        obj[ 'position' ] = { 'x': self.x, 'y': self.y, 'z': self.z }
        return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "xy"):
            if len( valueList ) != 2:
                raise KeyError( "AdaptPositionProcessor command \"xy\" must contain 2 floats" )
            # Account for the coordinate system of TouchOSC.
            # This assumes that both 'Invert X' and 'Invert Y' are active.
            self.y = float( valueList[0] )
            self.x = float( valueList[1] )
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )

        if (key == "id"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"id\" must contain 1 value." )
            self.objectId = int( valueList[0] )


        if (key == "height"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"height\" must contain 1 value." )
            self.z = float( valueList[0] )
