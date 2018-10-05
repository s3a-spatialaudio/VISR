# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

import abc

from .base_processor_interface import BaseProcessorInterface

class SequenceProcessorInterface(BaseProcessorInterface):
    """ Abstract base class for objects that adapt the object metadata vector """
    __metaclass__ = abc.ABCMeta
    def __init__(self, arguments ):
        # Common constructor code goes here. (nothing needs to be done at the time being)
        return

    @abc.abstractmethod
    def processObjectVector( self, objectVector ):
        """
        This method must be implemented by all derived MetaDataProcessor classes. """
        raise NotImplementedError( "SequenceProcessorInterface::processObjectVector() of abstract base class called" )

    @abc.abstractmethod
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        raise KeyError( "Abstract interface does SequenceProcessorInterface does not accept any parameters." )
