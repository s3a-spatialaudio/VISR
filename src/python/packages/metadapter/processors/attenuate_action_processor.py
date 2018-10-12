# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from distutils.util import strtobool
from copy import deepcopy

from metadapter import SequenceProcessorInterface

def GetCurrentGroup(obj):

    if 'groupname' in obj:
        current_group = obj['groupname']

    elif 'group' in obj:
        current_group = int(obj['group'])

    else:
        current_group = 0

    return current_group

class AttenuateActionProcessor(SequenceProcessorInterface):

    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        objStr = arguments.attrib['objectID']
        self.objectId = [int(s) for s in objStr.split() if s.isdigit()]
        self.active = strtobool( arguments.attrib['active'] )

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )

        objVectorNew = deepcopy(objectVector)
        for obj in objVectorNew:
            current_group = GetCurrentGroup(obj)
            #print current_group
            #if int( obj['id']) in self.objectId:
            if current_group == 3:
                #print "Adapting " + obj['id']
                if self.active:
                    obj['level'] = float(obj['level']) * 0.5
        return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptPositionProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )
            print ("Active: {}".format(self.active) )

        if (key == "objectID"):
            #Set new object list
            self.objectId = valueList
