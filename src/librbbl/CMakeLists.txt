# Copyright Institute of Sound and Vibration Research - All rights reserved


set( SOURCES 
biquad_coefficient.cpp
circular_buffer.cpp 
crossfading_convolver_uniform.cpp
core_convolver_uniform.cpp
fft_wrapper_factory.cpp
ffts_wrapper.cpp
fir.cpp
filter_routing.cpp
fractional_delay_base.cpp
fractional_delay_factory.cpp
float_sequence.cpp
gain_fader.cpp
gain_matrix.cpp
index_sequence.cpp
interpolating_convolver_uniform.cpp
interpolation_parameter.cpp
kiss_fft_wrapper_double.cpp
kiss_fft_wrapper_float.cpp
lagrange_interpolator.cpp
multichannel_convolver_uniform.cpp
multichannel_delay_line.cpp
object_channel_allocator.cpp
parametric_iir_coefficient.cpp
parametric_iir_coefficient_calculator.cpp
sparse_gain_routing.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
set( HEADERS
biquad_coefficient.hpp
circular_buffer.hpp
core_convolver_uniform.hpp
crossfading_convolver_uniform.hpp
export_symbols.hpp
filter_routing.hpp
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
interpolating_convolver_uniform.hpp
interpolation_parameter.hpp
kiss_fft_wrapper.hpp
lagrange_interpolator.hpp
multichannel_convolver_uniform.hpp
multichannel_delay_line.hpp
object_channel_allocator.hpp
parametric_iir_coefficient.hpp
parametric_iir_coefficient_calculator.hpp
sparse_gain_routing.hpp
)

if( BUILD_USE_IPP )
  list( APPEND SOURCES ipp_fft_wrapper.cpp )
  list( APPEND HEADERS ipp_fft_wrapper.hpp )
endif( BUILD_USE_IPP )

add_library( rbbl_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( rbbl_static PROPERTIES OUTPUT_NAME rbbl )

add_library( rbbl_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( rbbl_shared PROPERTIES OUTPUT_NAME rbbl )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( rbbl_shared PROPERTIES ARCHIVE_OUTPUT_NAME rbbl_shared )
target_compile_definitions( rbbl_shared PRIVATE -DVISR_BUILD_RBBL_SHARED_LIBRARY=1 -DVISR_BUILD_RBBL_LIBRARY=1)
target_compile_definitions( rbbl_shared PUBLIC -DVISR_BUILD_RBBL_SHARED_LIBRARY=1)
set_target_properties( rbbl_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# Set target properties common to both library types.
foreach(LIB_TYPE static shared)
  target_include_directories( rbbl_${LIB_TYPE} PRIVATE ${FFTS_INCLUDE_DIR} )
  target_include_directories( rbbl_${LIB_TYPE} PRIVATE ${KISSFFT_INCLUDE_DIR} )
  target_link_libraries( rbbl_${LIB_TYPE} PUBLIC efl_${LIB_TYPE} )
  target_link_libraries( rbbl_${LIB_TYPE} PRIVATE ${FFTS_LIBRARY} )
  target_link_libraries( rbbl_${LIB_TYPE} PRIVATE ${KISSFFT_LIBRARY} )
  target_link_libraries( rbbl_${LIB_TYPE} PUBLIC Boost::boost ) # Adds the boost include directory 
  if( BUILD_USE_IPP )
    target_compile_definitions( rbbl_${LIB_TYPE} PRIVATE -DBUILD_USE_IPP )
    target_include_directories( rbbl_${LIB_TYPE} PRIVATE ${IPP_INCLUDE_DIR} )
    target_link_libraries( rbbl_${LIB_TYPE} PRIVATE ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
  endif( BUILD_USE_IPP )
endforeach()

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( rbbl_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(rbbl_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS rbbl_static
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT static_libraries )
install(TARGETS rbbl_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files 
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/librbbl COMPONENT development_files )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
