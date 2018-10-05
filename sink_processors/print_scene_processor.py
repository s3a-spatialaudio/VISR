# -*- coding: utf-8 -*-
"""
Created on Fri Jan 06 10:00:00 2017

@author: af5u13
"""
from copy import deepcopy

from sink_processor_interface import SinkProcessorInterface

class PrintSceneProcessor(SinkProcessorInterface):
    def __init__(self, arguments ):
        SinkProcessorInterface.__init__(self, arguments)

    def consumeObjectVector( self, objectVector ):
        sortedVec = sorted( deepcopy( objectVector), key=lambda k: int(k['id']) )
        for o in sortedVec:
            # TODO: Pretty printing
            # TODO: Additional attribute filters.
            print( o )             

    def setParameter( self, key, valueList ):
        """ Empty implementation. Required because base class is an abstract base class."""
        raise KeyError( "No runtime parameters are supported." )
