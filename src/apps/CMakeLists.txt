# Copyright Institute of Sound and Vibration Research - All rights reserved

# Common base directory for all applications.


if( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "shared" ) AND NOT BUILD_INSTALL_SHARED_LIBRARIES )
  message( FATAL_ERROR
    "BUILD_LIBRARY_TYPE_FOR_APPS set to 'shared', but shared libraries are not built." )
endif( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "shared") AND NOT BUILD_INSTALL_SHARED_LIBRARIES )
if( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "static") AND NOT BUILD_INSTALL_STATIC_LIBRARIES )
  message( FATAL_ERROR
    "BUILD_LIBRARY_TYPE_FOR_APPS set to 'static', but static libraries are not built." )
endif( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "static") AND NOT BUILD_INSTALL_STATIC_LIBRARIES )

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
add_subdirectory( visr_renderer )
# To be continued.
