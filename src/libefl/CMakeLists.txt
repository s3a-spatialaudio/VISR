# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES denormalised_number_handling.cpp error_codes.cpp vector_conversions.cpp vector_functions.cpp )
SET( HEADERS aligned_array.hpp alignment.hpp basic_matrix.hpp denormalised_number_handling.hpp error_codes.hpp vector_conversions.hpp vector_functions.hpp )

ADD_LIBRARY( efl STATIC ${SOURCES} ${HEADERS} )
