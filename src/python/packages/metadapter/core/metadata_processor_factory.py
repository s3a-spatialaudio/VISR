# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

class MetadataProcessorFactory:
    processorTypeMap = {}

    """ An abstract factory to create MetadataProcessor objects based on their name """
    @classmethod
    def registerProcessorClass( self, processorName, ProcessorClass):
        self.processorTypeMap[processorName] = ProcessorClass;

    @classmethod
    def createProcessor( self, processorType, config ):
        if not processorType in self.processorTypeMap:
            raise KeyError( "Metadata processor type \"%s\" not known." % processorType )
        processorClass = self.processorTypeMap[processorType]
        newObj = processorClass( config )
        return newObj
