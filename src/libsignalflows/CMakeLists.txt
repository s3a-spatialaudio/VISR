# Copyright Institute of Sound and Vibration Research - All rights reserved

# Temporarily pass the path to baseline_renderer.cpp to compile in the hard-coded setup decoder configuration for the initial HOA support.
ADD_DEFINITIONS( -DCMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR} )

SET( SOURCES baseline_renderer.cpp delay_vector.cpp gain_matrix.cpp matrix_convolver.cpp)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS baseline_renderer.hpp delay_vector.hpp gain_matrix.hpp matrix_convolver.hpp )

ADD_LIBRARY( signalflows STATIC ${SOURCES} ${HEADERS} )
