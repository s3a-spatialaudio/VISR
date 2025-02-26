# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

import abc

from .base_processor_interface import BaseProcessorInterface

class ConditionProcessorInterface(BaseProcessorInterface):
    """ Abstract base class for processors acting as conditionals. """
    __metaclass__ = abc.ABCMeta
    def __init__(self, arguments ):
        # Common constructor code goes here. (nothing needs to be done at the time being)
        return

    @abc.abstractmethod
    def divideObjectVector( self, objectVector ):
        """
        Take the object vector and split it into object vectors for each branch (according to the conditional logic)
        """
        raise NotImplementedError( 'Pure virtual method "divideObjectVector""" not implemented in base class.')


    @abc.abstractmethod
    def numberOfBranches():
        """ Return the number of branches returned by the conditional"""
        raise NotImplementedError( 'Pure virtual method not implemented in base class.')

    @abc.abstractmethod
    def branchIds():
        """ Return the names of all branch ids in list of strings."""
        raise NotImplementedError( 'Pure virtual method not implemented in base class.')


    @abc.abstractmethod
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        raise KeyError( "Abstract interface does SequenceProcessorInterface does not accept any parameters." )
