# -*- coding: utf-8 -*-
"""
Created on Thu Jan 28 10:08:15 2016

@author: af5u13
"""

print( 'source_processors/__init__.py: Initialization called.' )

# File to ensure that the containing directory is treated as a Python module directory.

# Additional, use it to initialize the contents of the directory:
# Register the contained container classes.

from source_processors.source_processor_factory import SourceProcessorFactory

from source_processors.static_scene_source import StaticSceneSource

SourceProcessorFactory.registerProcessorClass( 'StaticSceneSource', StaticSceneSource )
print( 'source_processors/__init__.py: Initialization finished.' )
