# Copyright Institute of Sound and Vibration Research - All rights reserved

# Common base directory for all applications.

ADD_SUBDIRECTORY( baseline_renderer )
ADD_SUBDIRECTORY( feedthrough )
ADD_SUBDIRECTORY( gain_matrix )
ADD_SUBDIRECTORY( matrix_convolver )
ADD_SUBDIRECTORY( network_audio_streamer )
if( BUILD_PYTHON_BINDINGS )
  ADD_SUBDIRECTORY( python_runner )
endif( BUILD_PYTHON_BINDINGS )
ADD_SUBDIRECTORY( scene_decoder )
ADD_SUBDIRECTORY( time_frequency_feedthrough )
# To be continued.
