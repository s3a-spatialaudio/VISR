# -*- coding: utf-8 -*-
"""
Created on Thu Jan 28 10:08:15 2016

@author: af5u13
"""

# File to ensure that the containing directory is treated as a Python module directory.

# Additional, use it to initialize the contents of the directory:
# Register the contained container classes.

from merge_processors.merge_processor_factory import MergeProcessorFactory

from merge_processors.branch_order_merge_processor import BranchOrderMergeProcessor

print( 'merge_processors/__init__.py: Initialization called.' )
MergeProcessorFactory.registerProcessorClass( 'BranchOrderMerge', BranchOrderMergeProcessor )
print( 'merge_processors/__init__.py: Initialization finished.' )
