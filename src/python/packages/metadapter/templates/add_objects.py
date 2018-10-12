# -*- coding: utf-8 -*-
"""
Created on Mon Aug 08 17:10:05 2016

@author: af5u13
"""

# Template for a metadata adaptation processor that adds a set of objects to an
# object vector. This template is based on processors/add_objects_processor.py.

# Imports required for the generic functionality.
from copy import deepcopy
from itertools import groupby
# Import required for the specific example, delete if not used.
from json import loads

from sequence_processor_interface import SequenceProcessorInterface

# The processor class. 
# The name must be unique.
# The class must inherit from SequenceProcessorInterface, either directly (as here)
# or via an intermediate class.
# To use your derived class:
# - Place the file in processors/
# - Add the following code to processors/init_processor_library:
#   * In the import section above, add the following line (preserving the  alphabetic order)
#        from <file name without .py> import <class name>
#   * In the function initProcessorLibrary() add the call
#        MetadataProcessorFactory.registerProcessorClass( 'class name', class name )
class AddObjectsProcessor(SequenceProcessorInterface):
    # The init function can be used to setup all internal data members.
    # The parameter "arguments" contains the XML configuration for this 
    # processor as stored in the configuration file
    def __init__(self, arguments ):
        # Mandatory call to the base class' initialisation function
        SequenceProcessorInterface.__init__(self, arguments)
        # Any definition and initialisation of data local to the object 
        # goes here. This includes any consitency checking of the settings in
        # arguments.
        # <example implement starts here>
        # Build a set of object from the XML representation in the config file.
        objs = []
        # Read a JSON representations of the objects from the XML file.
        for objNode in arguments.iter( 'object' ):
            # Workaround to cope with the fact that double quoted (as used in JSON)
            # are not possible inside XML. So we use single quotes there and
            # substitute them here.
            data = objNode.attrib['data'].replace( '\'', '\"' )
            # Parse the JSON data into the Python object representation (hierarchical dictionaries)
            obj = loads( data )
            objs.append( obj )
        # Sort the objects according to their IDs to make addition to the object vetor easier.
        sortedObjs = sorted( objs, key=lambda k: int(k['id']) )
        # Create an object member variable containg the sorted objects to be added.
        self.additionalObjects = sortedObjs
        # <example implement ends here>

    # Implement the metadata adaptation.
    # The method is called periodically from the adaptation engine.
    # The argument "objectVector" contains current object vector (carrying the adaptations made in the previous stage) as a Python list,
    # The function must return an object vector (can be either the modified input argument, a deep copy or a newly constructed object vector.)
    def processObjectVector( self, objectVector):
        # Merge the incoming objects with the fixed and sort the resulting set by the id.
        # This makes duplicate element appear next to each other (the version from 
        # objectVector comes second as the sorting is stable) 
        # Note: we need to deepcopy the additionalObjects to avoid modifications 
        # to the original elements.
        # Note: Depending on the message sender (e.g., boost::property_tree), 
        # the id might actually be a string. So we need a special comparision function
        # that converts the id into an integer first.
        completeVector = sorted( deepcopy(self.additionalObjects) + objectVector,
                                 key=lambda k: int(k['id']) )
        # Group by identical ids and retain only the first object each.
        result = []
        for k, g in groupby(completeVector, lambda k: int(k['id']) ):
            result.append(g.next())
        # Return the newly created full object vector/
        return result[:]

    # the setParameter() method does not need to be overriden, because this processor 
    # does not implement run-time control message.
