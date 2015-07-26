# Cmake module for finding the FFTS library file.
#
# Inspired by FindSndFile.cmake
# Found on https://github.com/andresy/lua---sndfile/blob/master/cmake/FindSndFile.cmake
# Found on http://hg.kvats.net
#
# - Try to locate the FFTS library
# 
# Once done this will define
#
#  FFTS_FOUND - system has FFTS
#  FFTS_INCLUDE_DIRS - the FFTS include directory
#  FFTS_LIBRARIES - Link these to use FFTS

if (FFTS_LIBRARIES AND FFTS_INCLUDE_DIRS)
  # in cache already
  set(FFTS_FOUND TRUE)
else (FFTS_LIBRARIES AND FFTS_INCLUDE_DIRS)

  find_path(FFTS_INCLUDE_DIR
    NAMES
      ffts.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )
  
  find_library(FFTS_LIBRARY
    NAMES
      ffts_static
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(FFTS_INCLUDE_DIRS
    ${FFTS_INCLUDE_DIR}
  )
  set(FFTS_LIBRARIES
    ${FFTS_LIBRARY}
  )

  if (FFTS_INCLUDE_DIRS AND FFTS_LIBRARIES)
    set(FFTS_FOUND TRUE)
  endif (FFTS_INCLUDE_DIRS AND FFTS_LIBRARIES)

  if (FFTS_FOUND)
    if (NOT FFTS_FIND_QUIETLY)
      message(STATUS "Found FFTS: ${FFTS_LIBRARIES}")
    endif (NOT FFTS_FIND_QUIETLY)
  else (FFTS_FOUND)
    if (FFTS_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find FFTS")
    endif (FFTS_FIND_REQUIRED)
  endif (FFTS_FOUND)

  # show the FFTS_INCLUDE_DIRS and FFTS_LIBRARIES variables only in the advanced view
  mark_as_advanced(FFTS_INCLUDE_DIRS FFTS_LIBRARIES)

endif (FFTS_LIBRARIES AND FFTS_INCLUDE_DIRS)