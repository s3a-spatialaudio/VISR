# Copyright Institute of Sound and Vibration Research - All rights reserved

add_definitions( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

add_executable( visr_renderer main.cpp options.hpp options.cpp )

target_link_libraries(visr_renderer apputilities_static )
target_link_libraries(visr_renderer signalflows_static )
target_link_libraries(visr_renderer audiointerfaces_static )
target_link_libraries(visr_renderer rrl_static )
target_link_libraries(visr_renderer ${CMAKE_THREAD_LIBS_INIT} )
if( BUILD_PYTHON_BINDINGS )
  # Check whether this needs to be _shared (to get the initialisation right)  
  target_link_libraries(visr_renderer pythonsupport_static )
endif( BUILD_PYTHON_BINDINGS )
install( TARGETS visr_renderer DESTINATION bin )
