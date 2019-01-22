# Copyright Institute of Sound and Vibration Research - All rights reserved

add_executable( visr_renderer main.cpp options.hpp options.cpp )

target_compile_definitions( visr_renderer PRIVATE
  CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

target_link_libraries(visr_renderer apputilities_shared )
target_link_libraries(visr_renderer signalflowspython_shared )
target_link_libraries(visr_renderer audiointerfaces_shared )
target_link_libraries(visr_renderer panning_shared )
target_link_libraries(visr_renderer rrl_shared )
target_link_libraries(visr_renderer visr_shared )
target_link_libraries(visr_renderer Boost::filesystem )


if( BUILD_PYTHON_BINDINGS )
  # Note: pythonsupport must be linked as a shared library, because there must be
  # exactly one instance of pythonsupport::InitialisationGuard
  target_link_libraries(visr_renderer pythonsupport_shared )
  # Make sure that the Python interpreter initialisation is performed. 
  target_compile_definitions( visr_renderer PRIVATE VISR_PYTHON_SUPPORT=1 )
endif( BUILD_PYTHON_BINDINGS )

install( TARGETS visr_renderer
         RUNTIME DESTINATION bin 
         COMPONENT standalone_applications )
