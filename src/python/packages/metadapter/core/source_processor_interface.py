# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

import abc

from .base_processor_interface import BaseProcessorInterface

class SourceProcessorInterface(BaseProcessorInterface):
    """ Abstract base class for source processors. """
    __metaclass__ = abc.ABCMeta
    def __init__(self, arguments ):
        # Common constructor code goes here. (nothing needs to be done at the time being)
        return

    @abc.abstractmethod
    def createObjectVector( self ):
        """
        Take the object vector and split it into object vectors for each branch (according to the conditional logic)
        """
        raise NotImplementedError( 'Pure virtual method "divideObjectVector""" not implemented in base class.')

    # Note: the setParameter() method is inherited from BaseProcessorInterface