# -*- coding: utf-8 -*-
"""
Created on Fri Jan 06 10:00:00 2017

@author: af5u13
"""

from json import loads
from copy import deepcopy

from source_processor_interface import SourceProcessorInterface

class StaticSceneSource(SourceProcessorInterface):
    def __init__(self, arguments ):
        SourceProcessorInterface.__init__(self, arguments)
        objs = []
        for objNode in arguments.iter( 'object' ):
            data = objNode.attrib['data'].replace( '\'', '\"' )
            obj = loads( data )
            objs.append( obj )
        sortedObjs = sorted( objs, key=lambda k: int(k['id']) )
        self.staticScene = sortedObjs

    def createObjectVector( self ):
        return deepcopy( self.staticScene )

    def setParameter( self, key, valueList ):
        """ Empty implementation. Required because base class is an abstract base class."""
        raise KeyError( "No runtime parameters are supported." )
