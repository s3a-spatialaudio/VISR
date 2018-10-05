# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

from copy import deepcopy
import math
import socket
import random


from sys import version_info

if version_info.major <= 2:
    from OSC import OSCClient, OSCMessage
else:
    # Use the self-made port for Python 3 (experimental)
    from ..thirdparty.OSC import OSCClient, OSCMessage

#set up udp sender
#
# SPAT_IP = "127.0.0.1"
# SPAT_PORT = 5000
# SPAT_SOCK = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP

#Set up OSC client to send metadaper messages
client = OSCClient()
client.connect( ("localhost", 5001) )


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

class SpatMessageProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        objStr = arguments.attrib['objectID']
        self.objectId = [int(s) for s in objStr.split() if s.isdigit()]
        self.active = strtobool( arguments.attrib['active'] )

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )

        objVectorNew = deepcopy(objectVector)
        objIndex = 0
        if type(objVectorNew) is list:
            for obj in objVectorNew:
                if self.active:
                    if int( obj['id']) in self.objectId:
                        #objID = int(obj['id']) + 1
                        objIndex = objIndex + 1
                        if obj['type'] == 'plane':
                            az = float(obj['direction']['az'])
                            el = float(obj['direction']['el'])
                            r = float(obj['direction']['refdist'])
                            #radius set to 1 atm
                            #SPAT_msg = "source " + str(objID) + " aed " + str(-az) + " " + str(el) + " " + str(1)
                            #SPAT_SOCK.sendto(SPAT_msg, (SPAT_IP, SPAT_PORT))
                            SPAT_osc = OSCMessage()
                            SPAT_osc.setAddress('/SPAT/pos')
                            SPAT_osc.append([objIndex, -az, el, 1])
                            client.send(SPAT_osc)
                            #print "hi"
                        else:
                            x = float(obj[ 'position' ]['x'])
                            y = float(obj[ 'position' ]['y'])
                            z = float(obj[ 'position' ]['z'])
                            (r,az,el) = cart2sphDeg(x,y,z)
                            #SPAT_msg = "source " + str(objID) + " aed " + str(-az) + " " + str(el) + " " + str(1)
                            #SPAT_SOCK.sendto(SPAT_msg, (SPAT_IP, SPAT_PORT))
                            SPAT_osc = OSCMessage()
                            SPAT_osc.setAddress('/SPAT/pos')
                            SPAT_osc.append([objIndex, -az, el, 1])
                            client.send(SPAT_osc)
                            #print "hi"
        return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )
            print ("Active: {}".format(self.active))

        if (key == "objectID"):
            #Set new object list
            self.objectId = valueList

