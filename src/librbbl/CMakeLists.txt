# Copyright Institute of Sound and Vibration Research - All rights reserved


set( SOURCES 
biquad_coefficient.cpp
circular_buffer.cpp 
crossfading_convolver_uniform.cpp
fir.cpp
fft_wrapper_factory.cpp
ffts_wrapper.cpp
fractional_delay_base.cpp
fractional_delay_factory.cpp
float_sequence.cpp
gain_fader.cpp
gain_matrix.cpp
index_sequence.cpp
kiss_fft_wrapper_double.cpp
kiss_fft_wrapper_float.cpp
lagrange_interpolator.cpp
multichannel_convolver_uniform.cpp
multichannel_delay_line.cpp
object_channel_allocator.cpp
 parametric_iir_coefficient.cpp
parametric_iir_coefficient_calculator.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
set( HEADERS
biquad_coefficient.hpp
circular_buffer.hpp
crossfading_convolver_uniform.hpp
export_symbols.hpp
fir.hpp
fft_wrapper_base.hpp
fft_wrapper_factory.hpp
ffts_wrapper.hpp
float_sequence.hpp
fractional_delay_base.hpp
fractional_delay_factory.hpp
gain_fader.hpp
gain_matrix.hpp
index_sequence.hpp
kiss_fft_wrapper.hpp
lagrange_interpolator.hpp
multichannel_convolver_uniform.hpp
multichannel_delay_line.hpp
object_channel_allocator.hpp
parametric_iir_coefficient.hpp
parametric_iir_coefficient_calculator.hpp
)

include_directories( ${FFTS_INCLUDE_DIR} )
include_directories( ${KISSFFT_INCLUDE_DIR} )

if( BUILD_USE_FFTW )
  add_definitions( -DBUILD_USE_FFTW )
  include_directories( ${FFTW_INCLUDES} )
  list( APPEND SOURCES fftw_wrapper.cpp )
  list( APPEND HEADERS fftw_wrapper.hpp )
endif( BUILD_USE_FFTW )

if( BUILD_USE_IPP )
  add_definitions( -DBUILD_USE_IPP )
  include_directories( ${IPP_INCLUDE_DIR} )
  list( APPEND SOURCES ipp_fft_wrapper.cpp )
  list( APPEND HEADERS ipp_fft_wrapper.hpp )
endif( BUILD_USE_IPP )


add_library( rbbl_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( rbbl_static PROPERTIES OUTPUT_NAME rbbl )

target_link_libraries( rbbl_static efl_static )
target_link_libraries( rbbl_static pml_static )
target_link_libraries( rbbl_static ${FFTS_LIBRARY} )
target_link_libraries( rbbl_static ${KISSFFT_LIBRARY} )
if( BUILD_USE_FFTW )
  target_link_libraries( rbbl_static ${FFTW3_LIBRARY} )
  target_link_libraries( rbbl_static ${FFTW3F_LIBRARY} )
endif( BUILD_USE_FFTW )
if( BUILD_USE_IPP )
  target_link_libraries( rbbl_static ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
endif( BUILD_USE_IPP )

add_library( rbbl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rbbl_shared PROPERTIES OUTPUT_NAME rbbl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rbbl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rbbl_shared )
target_compile_definitions( rbbl_shared PRIVATE -DVISR_BUILD_RBBL_SHARED_LIBRARY=1 -DVISR_BUILD_RBBL_LIBRARY=1)
target_compile_definitions( rbbl_shared PUBLIC -DVISR_BUILD_RBBL_SHARED_LIBRARY=1)
set_target_properties( rbbl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( rbbl_shared efl_shared )
# ATTENTION: We cannot add the transitive link dependency to pml_shared here (although it exists), because CMake does not allow cyclic dependencies for shared library targets. Therefore, targets linking to rbbl_shared must add the dependency to pml_shared explicitly.
# NOTE: Restructure libraries to get rid of this cyclic dependency 
# target_link_libraries( rbbl_shared pml_shared )
target_link_libraries( rbbl_shared ${FFTS_LIBRARY} )
target_link_libraries( rbbl_shared ${KISSFFT_LIBRARY} )
if( BUILD_USE_FFTW )
  target_link_libraries( rbbl_shared ${FFTW3_LIBRARY} )
  target_link_libraries( rbbl_shared ${FFTW3F_LIBRARY} )
endif( BUILD_USE_FFTW )
if( BUILD_USE_IPP )
  target_link_libraries( rbbl_shared ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
endif( BUILD_USE_IPP )


# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( rbbl_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

install(TARGETS rbbl_static ARCHIVE DESTINATION lib )
install(TARGETS rbbl_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/visr/librbbl)

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
