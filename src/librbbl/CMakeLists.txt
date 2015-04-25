# Copyright Institute of Sound and Vibration Research - All rights reserved


SET( SOURCES circular_buffer.cpp fir.cpp  gain_matrix.cpp )

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS circular_buffer.hpp fir.hpp fft_wrapper_base.hpp gain_matrix.hpp )

IF( USE_FFTW )
  INCLUDE_DIRECTORIES( ${FFTW_INCLUDES} )
  LIST( APPEND SOURCES fftw_wrapper.cpp )
  LIST( APPEND HEADERS fftw_wrapper.hpp )
ENDIF( USE_FFTW )

ADD_LIBRARY( rbbl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
