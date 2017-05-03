# Copyright Institute of Sound and Vibration Research - All rights reserved

IF( BUILD_USE_IPP )
 INCLUDE_DIRECTORIES( ${IPP_INCLUDE_DIR} ) 
 LIST( APPEND SOURCES vector_functions_ipp.cpp)
ELSE( BUILD_USE_IPP )
 LIST( APPEND SOURCES vector_functions.cpp)
ENDIF( BUILD_USE_IPP )

SET( SOURCES
denormalised_number_handling.cpp
error_codes.cpp
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
matrix_functions.hpp
vector_conversions.hpp
vector_functions.hpp
vector_functions_reference.hpp
vector_functions_reference_impl.hpp
)

ADD_LIBRARY( efl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
