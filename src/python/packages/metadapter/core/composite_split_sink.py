# -*- coding: utf-8 -*-
"""
Created on Thu Jan 21 15:46:50 2016

@author: af5u13
"""

from .sequence_processor_interface import SequenceProcessorInterface

from .instantiate_subflow import instantiateSubFlow

class CompositeSplitSink(SequenceProcessorInterface):
    """ Abstract base class for objects that adapt the object metadata vector """
    def __init__(self, xmlConfig, processorLookup, verbose = False ):
        # Common constructor code goes here. (nothing needs to be done at the time being)

        # Lazy imports to avoid cyclic dependencies.
        from sink_processors.sink_processor_factory import SinkProcessorFactory
        from condition_processors.condition_processor_factory import ConditionProcessorFactory
        
        self.verbose = verbose

        self.conditionProcessor = None
        self.sequenceProcessors = []
        self.sinkProcessor = None

        subNodes = list( xmlConfig )
        for subNode in subNodes:
            if subNode.tag == 'condition':
                if (self.conditionProcessor != None):
                    raise ValueError( 'A split-sink construct must contain exactly one condition construct as the first sub-element' )
                conditionName = subNode.get( 'name' )
                conditionType = subNode.get( 'type' )
                self.conditionProcessor = ConditionProcessorFactory.createProcessor( conditionType, subNode )
                processorLookup[conditionName] = self.conditionProcessor
            elif subNode.tag == 'sink':
                pass
                if (self.conditionProcessor == None) or (self.sinkProcessor != None):
                   raise ValueError( 'A split-sink construct must contain exactly one merge processor as the last sub-element.' )
                sinkName = subNode.get( 'name' )
                sinkType = subNode.get( 'type' )
                self.sinkProcessor = SinkProcessorFactory.createProcessor( sinkType, subNode )
                processorLookup[sinkName] = self.sinkProcessor
            else:
                if (self.conditionProcessor == None) or (self.sinkProcessor != None):
                   raise ValueError( 'All sequence processors in a split-sink construct must be between the leading <condition> and the trailing <sink> processor.' )
                subElem = instantiateSubFlow( [subNode], processorLookup, verbose )
                self.sequenceProcessors.append( subElem )
        if (self.conditionProcessor == None) or (self.sinkProcessor == None):
            raise ValueError( 'A split-sink construct must contain exactly one condition and one sink processor.' )

        if self.conditionProcessor.numberOfBranches() != 2:
            raise ValueError( 'The condition processor in split-sink construct must contain exactly two branches.' )

        conditionBranchNames = self.conditionProcessor.branchIds()
        try:
            throughBranchName = xmlConfig.get( 'throughbranch' )
            sinkBranchName = xmlConfig.get( 'sinkbranch' )
        except BaseException:
            raise( 'A split-sink construct must contain the attributes "throughbranch" and "sinkbranch" to denote the names of the respective branches of the condition processor.' )

        if (not throughBranchName in conditionBranchNames) or (not sinkBranchName in conditionBranchNames ):
            raise( 'In a split-sink construct, the attributes "throughbranch" and "sourcebranch" must match the names of the branches of the merge processor.' )

        # Logical switch whether the source branch or the through brnach is passed first to the merge processor.
        self.sinkFirst = (sinkBranchName == conditionBranchNames[0])

    def processObjectVector( self, objectVector ):
        objVectors = self.conditionProcessor.divideObjectVector( objectVector )

        if len(objVectors) != 2:
            raise ValueError( 'The condition processor in a split-sink construct must return two object vectors.' )

        if self.sinkFirst:
            sinkVec = objVectors[0]
            throughVec = objVectors[1]
        else:
            sinkVec = objVectors[1]
            throughVec = objVectors[0]

        # Apply the optional sequence processing to the source-created objects.
        for proc in self.sequenceProcessors:
            sinkVec = proc.processObjectVector( sinkVec )

        # Perform the sink processing
        self.sinkProcessor.consumeObjectVector( sinkVec )
#
        return throughVec


    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        raise KeyError( "CompositeSourceMerge does not support runtime parameters." )
