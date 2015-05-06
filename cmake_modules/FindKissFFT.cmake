


if (KISSFFT_LIBRARIES AND KISSFFT_INCLUDE_DIRS)
  # in cache already
  set(KISSFFT_FOUND TRUE)
else (KISSFFT_LIBRARIES AND KISSFFT_INCLUDE_DIRS)

  find_path(KISSFFT_INCLUDE_DIR
    NAMES
      kiss_fft_float.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
  )
  
  find_library(KISSFFT_LIBRARY
    NAMES
      kissfft
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  set(KISSFFT_INCLUDE_DIRS
    ${KISSFFT_INCLUDE_DIR}
  )
  set(KISSFFT_LIBRARIES
    ${KISSFFT_LIBRARY}
  )

  if (KISSFFT_INCLUDE_DIRS AND KISSFFT_LIBRARIES)
    set(KISSFFT_FOUND TRUE)
  endif (KISSFFT_INCLUDE_DIRS AND KISSFFT_LIBRARIES)

  if (KISSFFT_FOUND)
    if (NOT KISSFFT_FIND_QUIETLY)
      message(STATUS "Found libkissfft: ${KISSFFT_LIBRARIES}")
    endif (NOT KISSFFT_FIND_QUIETLY)
  else (KISSFFT_FOUND)
    if (KISSFFT_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find libkissfft")
    endif (KISSFFT_FIND_REQUIRED)
  endif (KISSFFT_FOUND)

  # show the KISSFFT_INCLUDE_DIRS and KISSFFT_LIBRARIES variables only in the advanced view
  mark_as_advanced(KISSFFT_INCLUDE_DIRS KISSFFT_LIBRARIES)

endif (KISSFFT_LIBRARIES AND KISSFFT_INCLUDE_DIRS)