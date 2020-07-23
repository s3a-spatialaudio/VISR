# Copyright Institute of Sound and Vibration Research - All rights reserved

# Common base directory for all applications.

# Linking apps against shared libraries doesn't make sense if the shared libraries are not installed.
if( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "shared" ) AND NOT BUILD_INSTALL_SHARED_LIBRARIES )
  message( FATAL_ERROR
    "BUILD_LIBRARY_TYPE_FOR_APPS set to 'shared', but shared libraries are not installed." )
endif( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "shared") AND NOT BUILD_INSTALL_SHARED_LIBRARIES )

# For static libraries, we only need to ensure that they are built.
# This should always be true because the the "static" library type is automatically 
# added to the list if BUILD_LIBRARY_TYPE_FOR_APPS is set to this value.
if( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "static")
  AND NOT ( "static" IN_LIST VISR_BUILD_LIBRARY_TYPES) )
  message( FATAL_ERROR
    "BUILD_LIBRARY_TYPE_FOR_APPS set to 'static', but static libraries are not built." )
endif( (BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "static") 
  AND NOT ( "static" IN_LIST VISR_BUILD_LIBRARY_TYPES) )

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
