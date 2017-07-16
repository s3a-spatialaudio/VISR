# Copyright Institute of Sound and Vibration Research - All rights reserved


SET( SOURCES 
circular_buffer.cpp 
fir.cpp
fft_wrapper_factory.cpp
ffts_wrapper.cpp
fractional_delay_base.cpp
fractional_delay_factory.cpp
gain_fader.cpp
gain_matrix.cpp
kiss_fft_wrapper_double.cpp
kiss_fft_wrapper_float.cpp
lagrange_interpolator.cpp
multichannel_convolver_uniform.cpp
multichannel_delay_line.cpp
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
fractional_delay_base.hpp
fractional_delay_factory.hpp
gain_fader.hpp
gain_matrix.hpp
kiss_fft_wrapper.hpp
lagrange_interpolator.hpp
multichannel_convolver_uniform.hpp
multichannel_delay_line.hpp
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

target_link_libraries( rbbl ${FFTS_LIBRARY} )
target_link_libraries( rbbl ${KISSFFT_LIBRARY} )
if( BUILD_USE_FFTW )
  target_link_libraries( rbbl ${FFTW3_LIBRARY} )
  target_link_libraries( rbbl ${FFTW3F_LIBRARY} )
endif( BUILD_USE_FFTW )

if( BUILD_USE_IPP )
  target_link_libraries( rbbl ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
endif( BUILD_USE_IPP )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
