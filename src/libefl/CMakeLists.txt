# Copyright Institute of Sound and Vibration Research - All rights reserved

IF( BUILD_USE_IPP )
 INCLUDE_DIRECTORIES( ${IPP_INCLUDE_DIR} ) 
 LIST( APPEND SOURCES vector_functions_ipp.cpp)
ELSE( BUILD_USE_IPP )
 LIST( APPEND SOURCES vector_functions.cpp)
ENDIF( BUILD_USE_IPP )

SET( SOURCES
basic_matrix.cpp
basic_vector.cpp
denormalised_number_handling.cpp
error_codes.cpp
lagrange_coefficient_calculator.cpp
matrix_functions.cpp
vector_conversions.cpp
vector_functions_reference.cpp
vector_functions.cpp
)

SET( HEADERS
aligned_array.hpp
alignment.hpp
basic_matrix.hpp
basic_vector.hpp
cartesian_spherical_conversion.hpp
db_linear_conversion.hpp
degree_radian_conversion.hpp
denormalised_number_handling.hpp
error_codes.hpp
export_symbols.hpp
lagrange_coefficient_calculator.hpp
matrix_functions.hpp
vector_conversions.hpp
vector_functions.hpp
vector_functions_reference.hpp
vector_functions_reference_impl.hpp
)

add_library( efl_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( efl_static PROPERTIES OUTPUT_NAME efl )
target_compile_definitions( efl_static PUBLIC VISR_EFL_STATIC_LIBRARY=1)

add_library( efl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( efl_shared PROPERTIES OUTPUT_NAME efl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( efl_shared PROPERTIES ARCHIVE_OUTPUT_NAME efl_shared )
set_target_properties( efl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

foreach( LIB_TYPE static shared)
  target_compile_definitions( efl_${LIB_TYPE} PRIVATE VISR_BUILD_EFL_LIBRARY=1 )
endforeach()

# Set public headers to be installed for the shared library target.
# Because the headers are the same for the static library, the headers are only
# installed once.
set_target_properties( efl_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(efl_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

if( BUILD_INSTALL_STATIC_LIBRARIES )
  install(TARGETS efl_static
          EXPORT VISR_exports
          ARCHIVE DESTINATION lib COMPONENT static_libraries )
endif( BUILD_INSTALL_STATIC_LIBRARIES )
install(TARGETS efl_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/libefl COMPONENT development_files )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
