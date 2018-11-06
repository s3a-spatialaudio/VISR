# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

from .sequence_processor_interface import SequenceProcessorInterface

from .instantiate_subflow import instantiateSubFlow

class CompositeSourceMerge(SequenceProcessorInterface):
    """ Abstract base class for objects that adapt the object metadata vector """
    def __init__(self, xmlConfig, processorLookup, verbose = False ):
        # Common constructor code goes here. (nothing needs to be done at the time being)

        # Lazy imports to avoid cyclic dependencies.
        from source_processors.source_processor_factory import SourceProcessorFactory
        from merge_processors.merge_processor_factory import MergeProcessorFactory
        
        self.verbose = verbose

        self.sourceProcessor = None
        self.sequenceProcessors = []
        self.mergeProcessor = None

        subNodes = list( xmlConfig )
        for subNode in subNodes:
            if subNode.tag == 'source':
                if (self.sourceProcessor != None):
                    raise ValueError( 'A source-merge construct must contain exactly one source construct as the first sub-element' )
                sourceName = subNode.get( 'name' )
                sourceType = subNode.get( 'type' )
                self.sourceProcessor = SourceProcessorFactory.createProcessor( sourceType, subNode )
                processorLookup[sourceName] = self.sourceProcessor
            elif subNode.tag == 'merge':
                pass
                if (self.sourceProcessor == None) or (self.mergeProcessor != None):
                   raise ValueError( 'A source-merge construct must contain exactly one merge processor as the last sub-element.' )
                mergeName = subNode.get( 'name' )
                mergeType = subNode.get( 'type' )
                self.mergeProcessor = MergeProcessorFactory.createProcessor( mergeType, subNode )
                processorLookup[mergeName] = self.mergeProcessor
            else:
                if (self.sourceProcessor == None) or (self.mergeProcessor != None):
                   raise ValueError( 'All sequence processors in a source-merge construct must be between the leading <source> and the trailing <merge> processor.' )
                subElem = instantiateSubFlow( [subNode], processorLookup, verbose )
                self.sequenceProcessors.append( subElem )
        if (self.sourceProcessor == None) or (self.mergeProcessor == None):
            raise ValueError( 'A source-merge construct must contain exactly one source and one merge processor.' )

        if self.mergeProcessor.numberOfBranches() != 2:
            raise ValueError( 'The merge processor in source-merge construct must contain exactly two branches.' )

        mergeBranchNames = self.mergeProcessor.branchIds()
        try:
            throughBranchName = xmlConfig.get( 'throughbranch' )
            sourceBranchName = xmlConfig.get( 'sourcebranch' )
        except BaseException:
            raise( 'A source-merge construct must contain the attributes "throughbranch" and "sourcebranch" to denote the names of the respective branches of the merge processor.' )

        if (not throughBranchName in mergeBranchNames) or (not sourceBranchName in mergeBranchNames ):
            raise( 'In a source-merge construct, the attributes "throughbranch" and "sourcebranch" must match the names of the branches of the merge processor.' )

        # Logical switch whether the source branch or the through brnach is passed first to the merge processor.
        self.sourceFirst = (sourceBranchName == mergeBranchNames[0])

    def processObjectVector( self, objectVector ):
        # Execute the source processor
        sourceObjectVector = self.sourceProcessor.createObjectVector()

        # Apply the optional sequence processing to the source-created objects.
        for proc in self.sequenceProcessors:
            sourceObjectVector = proc.processObjectVector( sourceObjectVector )

        # There are only two possible orders how the two vectors can be passed into the merge, 
        # so we handle them explicitly.
        if self.sourceFirst:
            objVectors = [sourceObjectVector, objectVector]
        else:
            objVectors = [objectVector, sourceObjectVector ]
        # Perform the merge
        result = self.mergeProcessor.mergeObjectVectors( objVectors )
        return result

    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        raise KeyError( "CompositeSourceMerge does not support runtime parameters." )
