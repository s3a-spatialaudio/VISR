# -*- coding: utf-8 -*-
"""
Created on Thu Jan 28 10:08:15 2016

@author: af5u13
"""

# File to ensure that the containing directory is treated as a Python module directory.

# Additional, use it to initialize the contents of the directory:
# Register the contained container classes.

from ..core.metadata_processor_factory import MetadataProcessorFactory

from .addadvancedmetadata_processor import AddAdvancedMetadataProcessor
from .adapt_diffuseness_processor import AdaptDiffusenessProcessor
from .adapt_position_processor import AdaptPositionProcessor
from .add_mdo_metadata_processor import AddMdoMetadataProcessor
from .add_objects_processor import AddObjectsProcessor
from .add_reverb_processor import AddReverbProcessor
from .attenuate_action_processor import AttenuateActionProcessor
from .az_remap import AzMappingProcessor
from .change_volume_processor import ChangeVolumeProcessor
from .edit_distance_processor import EditDistanceProcessor
from .edit_reverb_processor import EditReverbProcessor
from .envelopment_processor import EnvelopmentProcessor
from .envelopmentopt_processor import EnvelopmentOptProcessor
from .fakegroups_processor import FakeGroupsProcessor
from .globaleq_processor import GlobalEQProcessor
from .mdo_processor import MDOProcessor
from .mdoProduction_processor import MDOProductionProcessor
#from .mdoAzimuth_processor import MDOAzimuthProcessor
#from .mdoPhones_processor import MDOPhonesProcessor
from .group_volume_processor import GroupVolumeProcessor
from .narrativeimportance_processor import NarrativeImportanceProcessor
from .routetosoundbar_processor import RouteToSoundbarProcessor
from .spread_processor import SpreadProcessor
from .solo_processor import SoloProcessor
from .group_adapt_radius_processor import GroupAdaptRadiusProcessor
from .Nuendo_height_fix_processor import NuendoHeightFixProcessor
from .compress_above_processor import CompressAboveProcessor
from .compress_below_processor import CompressBelowProcessor
from .compress_height_processor import CompressHeightProcessor
from .print_object_coordinate import PrintPositionProcessor
from .print_advanced_metadata_processor import PrintAdvancedMetadataProcessor
from .TMMGeneric_processor import TMMGenericProcessor
from .TMMExperiment3_MUSHRA_processor import TMMExperiment3_MUSHRAProcessor
from .spat_message_processor import SpatMessageProcessor
from .semantic_stereo_processor import SemanticStereoProcessor
#
#print( 'processors/__init__.py: Initialization called.' )
MetadataProcessorFactory.registerProcessorClass( 'AddAdvancedMetadata', AddAdvancedMetadataProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AdaptDiffuseness', AdaptDiffusenessProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AddMdoMetadata', AddMdoMetadataProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AddObjects', AddObjectsProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AdaptPosition', AdaptPositionProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AddReverb', AddReverbProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AttenuateAction', AttenuateActionProcessor )
MetadataProcessorFactory.registerProcessorClass( 'AzRemap', AzMappingProcessor )
MetadataProcessorFactory.registerProcessorClass( 'ChangeVolume', ChangeVolumeProcessor )
MetadataProcessorFactory.registerProcessorClass( 'EditDistance', EditDistanceProcessor )
MetadataProcessorFactory.registerProcessorClass( 'EditReverb', EditReverbProcessor )
MetadataProcessorFactory.registerProcessorClass( 'Envelopment', EnvelopmentProcessor )
MetadataProcessorFactory.registerProcessorClass( 'EnvelopmentOpt', EnvelopmentOptProcessor )
MetadataProcessorFactory.registerProcessorClass( 'FakeGroups', FakeGroupsProcessor )
MetadataProcessorFactory.registerProcessorClass( 'GlobalEQ', GlobalEQProcessor )
MetadataProcessorFactory.registerProcessorClass( 'MDO', MDOProcessor )
MetadataProcessorFactory.registerProcessorClass( 'MDOProduction', MDOProductionProcessor )
#MetadataProcessorFactory.registerProcessorClass( 'MDOAzimuth', MDOAzimuthProcessor )
#MetadataProcessorFactory.registerProcessorClass( 'MDOPhones', MDOPhonesProcessor )
MetadataProcessorFactory.registerProcessorClass( 'GroupVolume', GroupVolumeProcessor )
MetadataProcessorFactory.registerProcessorClass( 'NarrativeImportance', NarrativeImportanceProcessor )
MetadataProcessorFactory.registerProcessorClass( 'RouteToSoundbar', RouteToSoundbarProcessor )
MetadataProcessorFactory.registerProcessorClass( 'Spread', SpreadProcessor )
MetadataProcessorFactory.registerProcessorClass( 'Solo', SoloProcessor )
MetadataProcessorFactory.registerProcessorClass( 'CompressAbove', CompressAboveProcessor )
MetadataProcessorFactory.registerProcessorClass( 'CompressBelow', CompressBelowProcessor )
MetadataProcessorFactory.registerProcessorClass( 'CompressHeight', CompressHeightProcessor )
MetadataProcessorFactory.registerProcessorClass( 'NuendoHeightFix', NuendoHeightFixProcessor )
MetadataProcessorFactory.registerProcessorClass( 'PrintPosition', PrintPositionProcessor )
MetadataProcessorFactory.registerProcessorClass( 'GroupAdaptRadius', GroupAdaptRadiusProcessor )
MetadataProcessorFactory.registerProcessorClass( 'PrintAdvancedMetadata', PrintAdvancedMetadataProcessor )
MetadataProcessorFactory.registerProcessorClass( 'TMMGeneric', TMMGenericProcessor )
MetadataProcessorFactory.registerProcessorClass( 'TMMExperiment3_MUSHRA', TMMExperiment3_MUSHRAProcessor )
MetadataProcessorFactory.registerProcessorClass( 'SpatMessages', SpatMessageProcessor )
MetadataProcessorFactory.registerProcessorClass( 'SemanticStereo', SemanticStereoProcessor )
#print( 'processors/__init__.py: Initialization finished.' )
