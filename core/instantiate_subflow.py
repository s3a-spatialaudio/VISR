# -*- coding: utf-8 -*-
"""
Created on Tue Jan 03 22:58:12 2017

@author: af5u13
"""

# from .processors.metadata_processor_factory import MetadataProcessorFactory
from metadapter.core.metadata_processor_factory import MetadataProcessorFactory

def instantiateSubFlow( nodes, processorLookup, verbose = False ):
    """ Take a node of the XML configuration and instantiate a processor element corresponding to this subgraph.
    TODO: Maybe move to a different directory.
    TODO: Reconsider whether this should be a free function.
    """

    # Lazy imports to break circular import dependencies
    from .composite_conditional import CompositeConditional
    from .composite_sequence import CompositeSequence
    from .composite_source_merge import CompositeSourceMerge
    from .composite_split_sink import CompositeSplitSink

    if len( nodes ) == 0: # Empty parts are permissible, but require special treatment
        return None
    elif len( nodes ) > 1:
        newComposite = CompositeSequence( nodes, processorLookup, verbose )
        return newComposite
    else: # Exactly one element, handling depends on type
        singleNode = nodes[0];
        if singleNode.tag == 'processor':
            processorName = singleNode.get( 'name' )
            processorType = singleNode.get( 'type' )
            if processorName in processorLookup:
                raise ValueError( 'The processor name \"%s\" is not unique.'
                                  % processorName )
            if processorType == None:
                raise ValueError( 'The processor \"%s\" misses the required \"type\" attribute.' % processorType )
            if verbose:
                print( 'Found processor %s, type %s.' % (processorName, processorType ) )
            try:
                newProc = MetadataProcessorFactory.createProcessor( processorType, singleNode )
                if newProc == None:
                    raise RuntimeError( 'Processor type \"%s\" not found.' % processorType )
                processorLookup[processorName] = newProc
                return newProc
            except BaseException as ex:
                print( 'Exception while creating processor \"%s\": %s, skipping.' % ( processorName, ex.message ) )
            # For the moment, continue after the error
        elif singleNode.tag == 'sequence':
            # Other option: lift up the sequence by one level and invoke instantiateSubFlow directly (recursively)
            childNodes = list( singleNode )
            newComposite = CompositeSequence( childNodes, processorLookup, verbose )
            return newComposite
        elif singleNode.tag == 'conditional':
            newConditional = CompositeConditional( singleNode, processorLookup, verbose )
            return newConditional
        elif singleNode.tag == 'sourcemerge':
            newSourceMerge = CompositeSourceMerge( singleNode, processorLookup, verbose )
            return newSourceMerge
        elif singleNode.tag == 'splitsink':
            newSplitSink = CompositeSplitSink( singleNode, processorLookup, verbose )
            return newSplitSink
        else:
            raise RuntimeError( 'Unsupported structure element: <%s>.' % singleNode.tag )
