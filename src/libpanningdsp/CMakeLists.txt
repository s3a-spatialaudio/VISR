# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES
panning_gain_matrix.cpp
panning_matrix_parameter.cpp
)

set( HEADERS
export_symbols.hpp
panning_gain_matrix.hpp
panning_matrix_parameter.hpp
)

add_library( panningdsp_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( panningdsp_static PROPERTIES OUTPUT_NAME rcl )
target_compile_definitions( panningdsp_static PUBLIC VISR_PANNINGDSP_STATIC_LIBRARY=1)

add_library( panningdsp_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( panningdsp_shared PROPERTIES OUTPUT_NAME rcl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( panningdsp_shared PROPERTIES ARCHIVE_OUTPUT_NAME panningdsp_shared )
set_target_properties( panningdsp_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# Set target properties common to both library types.
foreach(LIB_TYPE static shared)
  target_compile_definitions( panningdsp_${LIB_TYPE} PRIVATE VISR_BUILD_PANNINGDSP_LIBRARY=1)
  target_link_libraries( panningdsp_${LIB_TYPE} PUBLIC pml_${LIB_TYPE} )
  target_link_libraries( panningdsp_${LIB_TYPE} PUBLIC visr_${LIB_TYPE} )
  target_link_libraries( panningdsp_${LIB_TYPE} PUBLIC efl_${LIB_TYPE} )
endforeach()

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties(panningdsp_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(panningdsp_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

if( BUILD_INSTALL_STATIC_LIBRARIES )
  install(TARGETS panningdsp_static
          EXPORT VISR_exports
          ARCHIVE DESTINATION lib COMPONENT static_libraries )
endif( BUILD_INSTALL_STATIC_LIBRARIES )
install(TARGETS panningdsp_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/librcl COMPONENT development_files )
