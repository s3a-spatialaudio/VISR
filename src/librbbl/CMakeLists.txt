# Copyright Institute of Sound and Vibration Research - All rights reserved


SET( SOURCES 
circular_buffer.cpp 
fir.cpp
fft_wrapper_factory.cpp
ffts_wrapper.cpp
gain_matrix.cpp
kiss_fft_wrapper_double.cpp
kiss_fft_wrapper_float.cpp
multichannel_convolver_uniform.cpp
object_channel_allocator.cpp
parametric_iir_coefficient_calculator.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS
circular_buffer.hpp
fir.hpp
fft_wrapper_base.hpp
fft_wrapper_factory.hpp
ffts_wrapper.hpp
gain_matrix.hpp
kiss_fft_wrapper.hpp
multichannel_convolver_uniform.hpp
object_channel_allocator.hpp
parametric_iir_coefficient_calculator.hpp
)

INCLUDE_DIRECTORIES( ${FFTS_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${KISSFFT_INCLUDE_DIR} )

IF( BUILD_USE_FFTW )
  ADD_DEFINITIONS( -DBUILD_USE_FFTW )
  INCLUDE_DIRECTORIES( ${FFTW_INCLUDES} )
  LIST( APPEND SOURCES fftw_wrapper.cpp )
  LIST( APPEND HEADERS fftw_wrapper.hpp )
ENDIF( BUILD_USE_FFTW )

IF( BUILD_USE_IPP )
  ADD_DEFINITIONS( -DBUILD_USE_IPP )
  INCLUDE_DIRECTORIES( ${IPP_INCLUDE_DIR} )
  LIST( APPEND SOURCES ipp_fft_wrapper.cpp )
  LIST( APPEND HEADERS ipp_fft_wrapper.hpp )
ENDIF( BUILD_USE_IPP )


ADD_LIBRARY( rbbl STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
