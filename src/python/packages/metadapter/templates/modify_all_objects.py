# -*- coding: utf-8 -*-
"""
Created on Mon Aug 08 17:10:05 2016

@author: af5u13
"""


# Template for changing properties of all contained data types.
# This example is derived from the ChangeVolumeProcessor class 
# (processors/change_volume_processor.py)

# Required for the example class, but not for the template.
import numpy

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
class ModifyAllObjectsProcessor(SequenceProcessorInterface):
    # The init function can be used to setup all internal data members.
    # The parameter "arguments" contains the XML configuration for this 
    # processor as stored in the configuration file
    def __init__(self, arguments ):
        # Mandatory call to the base class initialisation method
        SequenceProcessorInterface.__init__(self, arguments)
        # Any definition and initialisation of data local to the object 
        # goes here. This includes any consitency checking of the settings in
        # arguments.
        # <example implement starts>
        leveldB = float(arguments.attrib['volumeAdjust'])
        self.volumeChange = numpy.power( 10.0, leveldB/20.0 )
        # <example implement ends here>


    # Implement the metadata adaptation.
    # The method is called periodically from the adaptation engine.
    # The argument "objectVector" contains current object vector (carrying the adaptations made in the previous stage) as a Python list,
    # The function must return an object vector (can be either the modified input argument, a deep copy or a newly constructed object vector.)
    def processObjectVector( self, objectVector):
        # Iterate over all objects in the vector
        for obj in objectVector:
            # Apply an operation on all objects in the vector.
            # <example implement starts>
            obj['level'] = float(obj['level']) * self.volumeChange
            # <example implement ends here>
        # Return an object vector (in this case, the modified input argument)
        return objectVector


    # User defined method to handle incoming control data.
    # This method has to be implemented (overridden) if this processor class accepts external control data.
    # This method is called asynchronuously to the object adaptation because control data can arrive at arbitrary points in time. this implies that this method cannot directly influence the object vectors, but the received ontrol information has to be stored in the object's internal state (data members).
    # The argument 'key' is a string that denotes the type of a message. The meaning is specific to the processor and allows handling of multiple types of messages by a single object.
    # 'valueList' contains the parameters of this command as a Python list. Number, type, and semantics of the values are application-specific.
    def setParameter( self, key, valueList ):
        """ Set the parameter to a given value."""
        # Determine the key, and check the argument list based on number of elements, type of elements, and possibly also their values.
       # <example implement starts here>
        if (key != "volume") or (len( valueList ) != 1) or (not isinstance(valueList[0], float) ):
            raise KeyError( "ChangeVolumeProcessor supports only the parameter set command \"volume\" consisting of a single float value" )
        # <example implement ends here>
        # Apply the control data by changing member data of the object.
       # <example implement starts here>
        leveldB = valueList[0]
        self.volumeChange = numpy.power( 10.0, leveldB/20.0 )
       # <example implement ends here>
