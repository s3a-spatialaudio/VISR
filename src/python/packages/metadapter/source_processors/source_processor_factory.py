# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

# Not used at the moment.
# from sequence_processor_interface import SequenceProcessorInterface

class SourceProcessorFactory:
    processorTypeMap = {}

    """ An abstract factory to create MetadataProcessor objects based on their name """
    @classmethod
    def registerProcessorClass( self, processorName, ProcessorClass):
        print( 'Registering processor class %s' % processorName )
        self.processorTypeMap[processorName] = ProcessorClass;
        
    @classmethod
    def createProcessor( self, processorType, config ):
        if not processorType in self.processorTypeMap:
            raise KeyError( "Source processor type \"%s\" not known." % processorType )
        processorClass = self.processorTypeMap[processorType]
        newObj = processorClass( config )
        return newObj
