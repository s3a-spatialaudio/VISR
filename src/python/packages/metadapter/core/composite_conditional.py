# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

from .sequence_processor_interface import SequenceProcessorInterface

from .instantiate_subflow import instantiateSubFlow

# from condition_processors.condition_processor_factory import ConditionProcessorFactory

#class CompositeConditional:
#    """ Abstract base class for objects that adapt the object metadata vector """
#    def __init__(self, xmlConfig, processorLookup, verbose = False ):
#        pass

class CompositeConditional(SequenceProcessorInterface):
    """ Abstract base class for objects that adapt the object metadata vector """
    def __init__(self, xmlConfig, processorLookup, verbose = False ):
        # Common constructor code goes here. (nothing needs to be done at the time being)
    
        # Lazy imports to avoid cyclic dependencies.
        from condition_processors.condition_processor_factory import ConditionProcessorFactory
        from merge_processors.merge_processor_factory import MergeProcessorFactory
        
        # TODO: Move the construction code into a common facility (shared by all object types)
        self.verbose = verbose
        self.branches = [] # Self-managed list of child processors

        self.conditionProcessor = None
        self.branches = []
        self.mergeProcessor = None

        # Instantiate the condition 
        conditionNodes = xmlConfig.findall( 'condition' )
        if len(conditionNodes) != 1:
            raise ValueError( 'A conditional construct must contain exactly one <condition> element.' )
        conditionNode = conditionNodes[0]
        conditionName = conditionNode.get( 'name' )
        conditionType = conditionNode.get( 'type' )
        # Todo: Check uniqueness (or delegate insertion to lookup to a dedicated method)
        self.conditionProcessor = ConditionProcessorFactory.createProcessor( conditionType, conditionNode )
        processorLookup[conditionName] = self.conditionProcessor

        # Instantiate the branches
        branchNodes = xmlConfig.findall( 'branch' )
        numberOfBranches = len( self.branches )
        if numberOfBranches < 1:
            raise ValueError( 'Composite conditionals must contain at least one branch.' )
        self.branchNames = []
        
        for branchNode in branchNodes:
            branchName = branchNode.get( 'id' )
            self.branchNames.append( branchName )
            branchProcessors = list(branchNode)
            branchProcessingSequence = instantiateSubFlow( branchProcessors, processorLookup, verbose )
            self.branches.append( branchProcessingSequence )

            
        # Instantiate the merge processor
        # Special condition: If there is only one branch, we don't need a merge, i.e., it is optional
        mergeNodes = xmlConfig.findall( 'merge' )
        if len(mergeNodes) > 1:
            raise ValueError( 'A conditional construct must contain at most one <merge> element.' )
        if len(mergeNodes) == 0:
            if numberOfBranches > 1:
                raise ValueError( 'A conditional construct must contain one merge element if it has more than one branch.' )
        else:
            # Todo: Check uniqueness (or delegate insertion to lookup to a dedicated method)
            mergeNode = mergeNodes[0]
            mergeName = mergeNode.get( 'name' )
            mergeType = mergeNode.get( 'type' )
            self.mergeProcessor = MergeProcessorFactory.createProcessor( mergeType, mergeNode )
            processorLookup[mergeName] = self.mergeProcessor

            
        # Check whether the branch names (ids) used by the contition, the sub-branches, and the merger match.
        conditionBranchNames = self.conditionProcessor.branchIds()
        sortedBranchNames = sorted( self.branchNames )
        sortedConditionBranchNames = sorted( conditionBranchNames )        
        if sortedBranchNames != sortedConditionBranchNames:
            raise RuntimeError( 'The branch ids defined in the condition processor do not match the ids of the processing branches.')
        self.conditionBranchIndices = []
        for branchId in self.branchNames:
            conditionBranchId = conditionBranchNames.index( branchId )
            self.conditionBranchIndices.append( conditionBranchId )

        if self.mergeProcessor != None:            
            mergeBranchNames = self.mergeProcessor.branchIds()
            sortedMergeBranchNames = sorted( mergeBranchNames )
            if sortedBranchNames != sortedMergeBranchNames:
                raise RuntimeError( 'The branch ids defined in the merge processor do not match the ids of the processing branches.')        
            self.mergeBranchIndices = []
            for branchId in mergeBranchNames:
                processingBranchId = mergeBranchNames.index( branchId )
                self.mergeBranchIndices.append( processingBranchId )
            
    def processObjectVector( self, objectVector ):
        # Execute the condition
        branchedObjectVectors = self.conditionProcessor.divideObjectVector( objectVector )
        
        # Execute the branches
        branchResultVectors = []
        for branchIdx in range(0,len(self.branches)):
            partialObjVec = branchedObjectVectors[ self.conditionBranchIndices[branchIdx] ]
            if self.branches[branchIdx] == None: # Empty branch, pass the branch input on.
                partialResult = partialObjVec
            else:
                partialResult = self.branches[branchIdx].processObjectVector( partialObjVec )
            branchResultVectors.append( partialResult )
        
        if self.mergeProcessor == None: # If there is no merge, we have exactly one branch
            result = branchResultVectors[0] 
        else:
            # Reorder the list of object vectors expected by the merge processor.
            reorderedObjectVectors = [branchResultVectors[i] for i in self.mergeBranchIndices]
            # Perform the merge
            result = self.mergeProcessor.mergeObjectVectors( reorderedObjectVectors )
        return result
            
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        raise KeyError( "Concrete class CompositeSequence does not support runtime parameters." )
