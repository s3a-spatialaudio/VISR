# - Find FFTW
# Find the native FFTW includes and library
#
#  FFTW_INCLUDES    - where to find fftw3.h
#  FFTW_LIBRARIES   - List of libraries when using FFTW.
#  FFTW_FOUND       - True if FFTW found.
# Taken from https://github.com/jedbrown/cmake-modules/blob/master/FindFFTW.cmake
# Adaptations (AF):
# - Provide all three flavours of the library (double, float, long double)
# - Add FFTW_LIBRARIES variable to collect the three libraries into a single variable (not working yet)


if (FFTW_INCLUDES)
  # Already in cache, be silent
  set (FFTW_FIND_QUIETLY TRUE)
endif (FFTW_INCLUDES)

find_path(FFTW_INCLUDES fftw3.h)

find_library(FFTW3_LIBRARY NAMES fftw3)
find_library(FFTW3F_LIBRARY NAMES fftw3f)
find_library(FFTW3L_LIBRARY NAMES fftw3l)

SET( FFTW_LIBRARIES FFTW3_LIBRARY FFTW3F_LIBRARY FFTW3L_LIBRARY )


# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDES)

mark_as_advanced( FFTW_LIBRARIES FFTW_INCLUDES FFTW3_LIBRARY FFTW3F_LIBRARY FFTW3L_LIBRARY)
