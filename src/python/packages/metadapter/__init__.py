# Copyright Andreas Franck 2016-2019 - All rights reserved
# Copyright Institute of Sound and Vibration Research - All rights reserved

# Import these interface classes into the main metadapter mainspace
from .core.metadata_processor_factory import MetadataProcessorFactory
from .core.base_processor_interface import BaseProcessorInterface
from .core.condition_processor_interface import ConditionProcessorInterface
from .core.merge_processor_interface import MergeProcessorInterface
from .core.sequence_processor_interface import SequenceProcessorInterface
from .core.sink_processor_interface import SinkProcessorInterface
from .core.source_processor_interface import SourceProcessorInterface

from . import processors

# Technically, they wouldn't need to be known externally
# TODO: Consider moving them at least into the "core" subnamespace.
from .core.metadapter_engine import Engine
from .core.instantiate_subflow import instantiateSubFlow
from .core.composite_conditional import CompositeConditional
from .core.composite_sequence import CompositeSequence
from .core.composite_source_merge import CompositeSourceMerge
from .core.composite_split_sink import CompositeSplitSink

# Do not automatically import the visr subdirectory here.
# This means it has to be imported as a nested subpackage, e.g., "import metadapter.visr"
# Reason: In this way we avoid the dependency on the VISR framework packages unless the
# metadapter is used as a VISR component.

# Do not automatically import the "networked" subdirectory here.
# This means it has to be imported as a nested subpackage, e.g., "import metadapter.networked"
# This is to avoid the dependencies specific to this way of using the metadapter, for example
# the "twisted" package

from .thirdparty import OSC as OSC
