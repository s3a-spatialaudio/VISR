# compile mex

# Code adapted from
# http://www.mathworks.com/matlabcentral/fileexchange/45522-mex-cmake

set(PROJECT_NAME feedthrough_mex)

set(CPP_FILE feedthrough)

set( ADDITIONAL_CPP_FILES signal_flow.cpp )

# set up matlab libraries
set(CPP_FILE_NAME ${CPP_FILE}.cpp)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

# <AF> Don't use export files, put export the mexFunction explicitly.
# <AF> Consider replacing SHARED with MODULE
add_library(${PROJECT_NAME} MODULE ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES}  ${CMAKE_SOURCE_DIR}/cmake_modules/Matlabdef.def )

# Add any extras dependencies
target_link_libraries( ${PROJECT_NAME} rcl_shared )
target_link_libraries( ${PROJECT_NAME} visr_shared )
target_link_libraries( ${PROJECT_NAME} rrl_shared )
target_link_libraries( ${PROJECT_NAME} mexsupport )

# Set the name of the mex object (and thus the Matlab function to be called) independently
# of the project name.
# Also set the file extension according to the operating system.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} SUFFIX ${MATLAB_MEX_SUFFIX} PREFIX "")

# Install the .mexXXX file to mex/
INSTALL(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION mex)
