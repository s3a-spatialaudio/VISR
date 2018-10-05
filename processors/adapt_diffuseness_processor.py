# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from distutils.util import strtobool

from metadapter import SequenceProcessorInterface

from copy import deepcopy

class AdaptDiffusenessProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)

        if 'id' in arguments.attrib:
            self.id = int( arguments.attrib['id'] )
        else:
            self.id = 0

        if 'active' in arguments.attrib:
            self.active = strtobool( arguments.attrib['active'] )
        else:
            self.active = 0

        if 'diffuse' in arguments.attrib:
            self.diffuse = float( arguments.attrib['diffuse'] )
        else:
            self.diffuse = 0

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )

        objVectorNew = deepcopy(objectVector)
        for obj in objVectorNew:
            if int( obj['id']) == self.id:
                if self.active:
                    if obj['type'] == 'pointdiffuse':
                        obj['diffuseness'] = self.diffuse
        return objVectorNew

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key == "diffuse"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptDiffusenessProcessor command \"diffuse\" must contain 1 float" )
            self.diffuse = float( valueList[0] )
        if (key == "active"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptDiffusenessProcessor command \"active\" must contain 1 value." )
            self.active = bool( valueList[0] )

        if (key == "id"):
            if len( valueList ) != 1:
                raise KeyError( "AdaptDiffusenessProcessor command \"id\" must contain 1 value." )
            self.id = int( valueList[0] )
