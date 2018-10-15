# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 17:10:05 2016

@author: af5u13
"""

import re

from condition_processor_interface import ConditionProcessorInterface

class SelectGroupConditionProcessor(ConditionProcessorInterface):
    def __init__(self, arguments ):
        ConditionProcessorInterface.__init__(self, arguments)
        self.groupLookup = {}
        self.wildCardBranches = []
        self.branchNames = []
        branches = arguments.findall( 'group' )
        branchIndex = 0
        for branchIter in branches:
            branchName = branchIter.get( 'branch' )
            # TODO: Check if name already exists
            self.branchNames.append( branchName )
            groupString  = branchIter.get( 'groupIds' )
            if groupString == '*':
                self.wildCardBranches.append( branchIndex )
            else:
                groups = [int(x) for x in re.split( ' *, *', groupString ) ]
                for groupIt in groups:
                    if not self.groupLookup.has_key( groupIt ):
                        self.groupLookup[ groupIt ] = []
                    self.groupLookup[ groupIt ].append( branchIndex )
            branchIndex = branchIndex + 1
        # Handle the default branch
        defaultBranch = arguments.findall( 'default' )
        if len(defaultBranch) == 0:
            self.defaultBranch = None
        elif len(defaultBranch) > 1:
            raise RuntimeError( 'At most one default branch is permitted.' )
        else:
            branchName = defaultBranch[0].get( 'branch' )
            self.branchNames.append( branchName )
            self.defaultBranch = branchIndex
            
#        sortedObjs = sorted( objs, key=lambda k: int(k['id']) 

    def divideObjectVector( self, objectVector):
        # Create an empty list for each output branch
        result = [ [] for _ in range(0,self.numberOfBranches() ) ]
        for o in objectVector:
            gid = o['group']
            for wc in self.wildCardBranches:
                result[wc].append( o )
            if gid in self.groupLookup:
                branchIds = self.groupLookup[gid]
                for branchId in branchIds:
                    result[branchId].append( o )
            elif self.defaultBranch != None:
                result[self.defaultBranch].append( o )
        return result
        
    def branchIds(self):
        return self.branchNames
        
    def numberOfBranches(self):
        return len( self.branchNames )

    def setParameter( self, key, valueList ):
        """ Empty implementation. Required because base class is an abstract base class."""
        raise KeyError( "No runtime parameters are supported." )
