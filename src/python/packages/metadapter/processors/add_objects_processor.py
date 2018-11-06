# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from copy import deepcopy
from json import loads
from itertools import groupby

from metadapter import SequenceProcessorInterface

class AddObjectsProcessor(SequenceProcessorInterface):
    def __init__(self, arguments ):
        SequenceProcessorInterface.__init__(self, arguments)
        objs = []
        for objNode in arguments.iter( 'object' ):
            data = objNode.attrib['data'].replace( '\'', '\"' )
            obj = loads( data )
            objs.append( obj )
        sortedObjs = sorted( objs, key=lambda k: int(k['id']) )
        self.additionalObjects = sortedObjs

    def processObjectVector( self, objectVector):
        # Function to be implemented by all derived MetaDataProcessor interfaces.

        # Merge and remove duplicates.
        # Sorts the input vector as a side effect.
        # This implementation relies that sorted() is stable.
        # TODO: Check whether the sorting works as intended.

        # Note: we need to deepcopy the additionalObjects to avoid modifications
        # to the original elements.
        # Note: Depending on the message sender (e.g., boost::property_tree),
        # the id might actually be a string. So we need a special comparison function.
        completeVector = sorted( deepcopy(self.additionalObjects) + objectVector,
                                 key=lambda k: int(k['id']) )
        result = []
        for k, g in groupby(completeVector, lambda k: int(k['id']) ):
            result.append( next(g) )

        return result[:]

    def setParameter( self, key, valueList ):
        """ Empty implementation. Required because base class is an abstract base class."""
        raise KeyError( "No runtime parameters are supported." )
