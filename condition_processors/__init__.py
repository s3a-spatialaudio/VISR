# -*- coding: utf-8 -*-
"""
Created on Thu Jan 28 10:08:15 2016

@author: af5u13
"""

# File to ensure that the containing directory is treated as a Python module directory.

# Additional, use it to initialize the contents of the directory:
# Register the contained container classes.

from condition_processors.condition_processor_factory import ConditionProcessorFactory

from condition_processors.select_group_condition import SelectGroupConditionProcessor

print( 'condition_processors/__init__.py: Initialization called.' )
ConditionProcessorFactory.registerProcessorClass( 'SelectGroup', SelectGroupConditionProcessor )
print( 'condition_processors/__init__.py: Initialization finished.' )
