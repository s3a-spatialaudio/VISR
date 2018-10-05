# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

import math

from metadapter import SequenceProcessorInterface

def lin2dB( linVal ):
    return 20.0 * math.log10( linVal )
    
def dB2lin( dbVal ):
    return math.pow( 10.0, 0.05 * dbVal )

class GroupVolumeProcessor(SequenceProcessorInterface):
    
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        self.objectLookup = {}
        self.volumes = {}
        for groupNode in arguments.iter( 'group' ):
            groupName = groupNode.attrib['name']
            objsStr = groupNode.attrib['objectIds']
            initialVolume = dB2lin( float(groupNode.attrib['initialVolume']) )
            objIds = [int(s) for s in objsStr.split() if s.isdigit()]
            for objId in objIds:
                self.objectLookup[objId] = groupName
            self.volumes[groupName] = initialVolume

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        for obj in objectVector:
            id = int( obj['id'] )
            if id in self.objectLookup:
                group = self.objectLookup[id]
                adjustVolume = self.volumes[group]
                obj['level'] = float( obj['level'] ) * adjustVolume
        return objectVector
        
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
            raise KeyError( "GroupVolumeProcessor supports only commands consisting of a single float value" )
        if not key in self.volumes:
            raise KeyError( "GroupVolumeProcessor: The command name does not correspond to a known group." )
        leveldB = valueList[0]
        self.volumes[key] = dB2lin( leveldB )