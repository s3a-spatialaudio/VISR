# Copyright Institute of Sound and Vibration Research - All rights reserved


set( HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/cpu_features.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/emulate_addsub.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/initialise_library.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vector_functions.hpp
)

set( SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/cpu_features.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/initialise_library.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vector_multiply.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vector_multiply_add.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/vector_ramp_scaling.cpp
)

# Add the architecture-specific files to the efl libraries.
foreach(TARGET_TYPE static shared)
  target_sources( efl_${TARGET_TYPE} PRIVATE ${HEADERS} ${SOURCES} )
endforeach()
