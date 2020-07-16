cmake_minimum_required( VERSION 2.8.11 )

project( kissFFT LANGUAGES C )

set( SOURCES
  kiss_fft_double.c
  kiss_fft_float.c
  kiss_fftr_double.c
  kiss_fftr_float.c
)

set( PUBLIC_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fft.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fft_double.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fft_double_redefines.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fft_float.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fft_float_redefines.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fftr_double.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fftr_double_redefines.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fftr_float.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kiss_fftr_float_redefines.h
)

add_library( kissfft_static STATIC ${SOURCES} )
# This is necessary because files in tools/ include headers in the main
# directory using #include "" without a relative path.
# Note At the moment, thses are defined only as PRIVATE, because the public and
#interface declarations are done externally (because we include kissfft as an
# add_subdirectory() subproject.)
target_include_directories( kissfft_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# Set position-independent code for the static library to make it usable in shared modules.
set_property( TARGET kissfft_static PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property( TARGET kissfft_static PROPERTY PUBLIC_HEADER "${PUBLIC_HEADERS}" )
set_property( TARGET kissfft_static PROPERTY ARCHIVE_OUTPUT_NAME kissfft )

add_library( kissfft_shared SHARED ${SOURCES} )
# This is necessary because files in tools/ include headers in the main
# directory using #include "" without a relative path.
target_include_directories( kissfft_shared PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_property( TARGET kissfft_shared PROPERTY PUBLIC_HEADER "${PUBLIC_HEADERS}" )
# Export all shared library (because we don't have sophisticated per-symbol
# declarations)
set_property( TARGET kissfft_shared PROPERTY C_VISIBILITY_PRESET default )
set_property( TARGET kissfft_shared PROPERTY LIBRARY_OUTPUT_NAME kissfft )
