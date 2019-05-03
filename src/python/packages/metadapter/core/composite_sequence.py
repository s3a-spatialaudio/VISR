# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

from .sequence_processor_interface import SequenceProcessorInterface

from .instantiate_subflow import instantiateSubFlow

class CompositeSequence(SequenceProcessorInterface):
    """ Abstract base class for objects that adapt the object metadata vector """
    def __init__(self, xmlConfig, processorLookup, verbose = False ):
        self.verbose = verbose
        self.processorList = [] # Self-managed list of child processors
        for processorChild in xmlConfig:
            singleChildList = [ processorChild ] # The construction function expects a list, even if it is single-element.
            subProcessor = instantiateSubFlow( singleChildList, processorLookup, verbose )
            self.processorList.append( subProcessor )

    def processObjectVector( self, objectVector ):
        """
        This method must be implemented by all derived MetaDataProcessor classes.
        """
        for proc in self.processorList:
            objectVector = proc.processObjectVector( objectVector )
        return objectVector

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        raise KeyError( "Concrete class CompositeSequence does not support runtime parameters." )
