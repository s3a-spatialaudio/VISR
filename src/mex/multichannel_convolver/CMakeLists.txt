# compile mex

# Code adapted from
# http://www.mathworks.com/matlabcentral/fileexchange/45522-mex-cmake

set(CPP_FILE multichannel_convolver )

set(HEADER_FILE utility_functions.hpp )

set(PROJECT_NAME multichannel_convolver_mex )

set( ADDITIONAL_CPP_FILES utility_functions.cpp )

# set up matlab libraries
set(CPP_FILE_NAME ${CPP_FILE}.cpp)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

# <AF> Don't use export files, put export the mexFunction explicitly.
# <AF> Consider replacing SHARED with MODULE
add_library(${PROJECT_NAME} SHARED ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES}  ${CMAKE_SOURCE_DIR}/cmake_modules/Matlabdef.def )

# Set the name of the mex object (and thus the Matlab function to be called) independently
# of the project name.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} )

# Add any extras dependencies
TARGET_LINK_LIBRARIES(${PROJECT_NAME} rbbl rcl ril efl ${MATLAB_LIBRARIES})

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${SNDFILE_LIBRARIES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${FFTS_LIBRARY} )

IF( BUILD_USE_FFTW )
  TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${FFTW3_LIBRARY}  ${FFTW3F_LIBRARY})
ENDIF( BUILD_USE_FFTW )

# <AF> TODO: Move this conditional logic to a centralised loacation!
# 32-bit or 64-bit mex
if(WIN32)
  if (CMAKE_CL_64)
      SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX .mexw64)
  else(CMAKE_CL_64)
      SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX .mexw32)
  endif(CMAKE_CL_64)
else(WIN32)
  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX .mexa64 PREFIX "")
  else(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX .mexglx PREFIX "")
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
endif(WIN32)
# SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES SUFFIX ${MATLAB_SUFFIX_MEX} )

# <AF> What to do with the install?
# install to /bin by default
install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_BINARY_DIR}/mex )
