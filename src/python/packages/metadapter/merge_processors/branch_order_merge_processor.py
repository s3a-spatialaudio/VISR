# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

from itertools import chain
from itertools import groupby

from merge_processor_interface import MergeProcessorInterface

class BranchOrderMergeProcessor(MergeProcessorInterface):
    def __init__(self, arguments ):
        MergeProcessorInterface.__init__(self, arguments)
        orderString = arguments.get( 'order' )
        if orderString == None:
            raise RuntimeError( 'BranchOrderMergeProcessor: Required attribute "order" is missing.')
        self.branchNames = [x for x in orderString.split(',') ]

    def mergeObjectVectors( self, objectVectors):
        # Create an empty list for each output branch
        flatList = chain.from_iterable( objectVectors )
        # Rely that the sort function is stable.
        sortedVector = sorted( flatList,
                                 key=lambda k: int(k['id']) )
        result = []
        # Remove all duplicates after the first occurence of an object id.
        for k, g in groupby(sortedVector, lambda k: int(k['id']) ):
            result.append( next(g) )
        return result
        
    def branchIds(self):
        return self.branchNames
        
    def numberOfBranches(self):
        return len( self.branchNames )

    def setParameter( self, key, valueList ):
        """ Empty implementation. Required because base class is an abstract base class."""
        raise KeyError( "No runtime parameters are supported." )
