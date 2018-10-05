#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar  8 16:17:47 2018

@author: af5u13
"""

from .core.metadapter_engine import Engine
from .core.instantiate_subflow import instantiateSubFlow

from .core.metadata_processor_factory import MetadataProcessorFactory

# Technically, they wouldn't need to be known externally
from .core.composite_conditional import CompositeConditional
from .core.composite_sequence import CompositeSequence
from .core.composite_source_merge import CompositeSourceMerge
from .core.composite_split_sink import CompositeSplitSink

from .core.base_processor_interface import BaseProcessorInterface
from .core.condition_processor_interface import ConditionProcessorInterface
from .core.merge_processor_interface import MergeProcessorInterface
from .core.sequence_processor_interface import SequenceProcessorInterface
from .core.sink_processor_interface import SinkProcessorInterface
from .core.source_processor_interface import SourceProcessorInterface

from .visr.component import Component

from . import processors

from .thirdparty import OSC as OSC
