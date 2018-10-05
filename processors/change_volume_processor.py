# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

import numpy

from metadapter import SequenceProcessorInterface

class ChangeVolumeProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        leveldB = float(arguments.attrib['volumeAdjust'])
        self.volumeChange = numpy.power( 10.0, leveldB/20.0 )

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.
        # print( "ChangeVolumeProcessor::processObjectVector() called." )
        for obj in objectVector:
            obj['level'] = float(obj['level']) * self.volumeChange
        return objectVector

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        if (key != "volume") or (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
            raise KeyError( "ChangeVolumeProcessor supports only the parameter set command \"volume\" consisting of a single float value" )
        leveldB = valueList[0]
        self.volumeChange = numpy.power( 10.0, leveldB/20.0 )
