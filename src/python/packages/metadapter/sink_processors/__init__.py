# -*- coding: utf-8 -*-
"""
Created on Thu Jan 28 10:08:15 2016

@author: af5u13
"""

print( 'sink_processors/__init__.py: Initialization called.' )

# File to ensure that the containing directory is treated as a Python module directory.

# Additional, use it to initialize the contents of the directory:
# Register the contained container classes.

from sink_processors.sink_processor_factory import SinkProcessorFactory

from sink_processors.print_scene_processor import PrintSceneProcessor

SinkProcessorFactory.registerProcessorClass( 'PrintSceneProcessor', PrintSceneProcessor )
print( 'sink_processors/__init__.py: Initialization finished.' )
