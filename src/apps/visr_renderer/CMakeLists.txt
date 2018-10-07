# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( visr_renderer main.cpp options.hpp options.cpp )

target_compile_definitions( visr_renderer PRIVATE
  CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

target_link_libraries(visr_renderer apputilities_static )
# target_link_libraries(visr_renderer signalflows_static )
target_link_libraries(visr_renderer signalflowspython_static )
target_link_libraries(visr_renderer audiointerfaces_static )
target_link_libraries(visr_renderer panning_static )
target_link_libraries(visr_renderer rrl_static )
target_link_libraries(visr_renderer ${CMAKE_THREAD_LIBS_INIT} )

if( BUILD_PYTHON_BINDINGS )
  # Note: pythonsupport must be linked as a shared library, because there must be
  # exactly one instance of pythonsupport::InitialisationGuard
  target_link_libraries(visr_renderer pythonsupport_shared )
  # Make sure that the Python interpreter initialisation is performed. 
  target_compile_definitions( visr_renderer PRIVATE VISR_PYTHON_SUPPORT=1 )
endif( BUILD_PYTHON_BINDINGS )

install( TARGETS visr_renderer RUNTIME DESTINATION bin COMPONENT standalone_applications )
