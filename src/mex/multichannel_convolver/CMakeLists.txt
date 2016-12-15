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
add_library(${PROJECT_NAME} MODULE ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES}  ${CMAKE_SOURCE_DIR}/cmake_modules/Matlabdef.def )

# Set the name of the mex object (and thus the Matlab function to be called) independently
# of the project name.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} )

# Add any extras dependencies
TARGET_LINK_LIBRARIES(${PROJECT_NAME} rbbl pml rcl ril efl ${MATLAB_LIBRARIES})

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${SNDFILE_LIBRARIES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${FFTS_LIBRARY} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${FFTS_LIBRARY} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${KISSFFT_LIBRARY} )
IF( BUILD_USE_FFTW )
  TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${FFTW3_LIBRARY}  ${FFTW3F_LIBRARY})
ENDIF( BUILD_USE_FFTW )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${KISSFFT_LIBRARY} )
IF( BUILD_USE_IPP )
  TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
ENDIF( BUILD_USE_IPP )

# Set the name of the mex object (and thus the Matlab function to be called) independently
# of the project name.
# Also set the file extension according to the operating system.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} SUFFIX ${MATLAB_MEX_SUFFIX} PREFIX "")

# Install the .mexXXX file to mex/
INSTALL(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION mex )
