# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES error_codes.cpp vector_conversions.cpp vector_functions.cpp )
SET( HEADERS aligned_array.hpp alignment.hpp error_codes.hpp vector_conversions.hpp vector_functions.hpp )

ADD_LIBRARY( efl STATIC ${SOURCES} ${HEADERS} )
