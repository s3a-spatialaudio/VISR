# compile mex

# Code adapted from
# http://www.mathworks.com/matlabcentral/fileexchange/45522-mex-cmake

set(CPP_FILE feedthrough)

set( ADDITIONAL_CPP_FILES mex_wrapper.cpp signal_flow.cpp )

# set up matlab libraries
set(CPP_FILE_NAME ${CPP_FILE}.cpp)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

# <AF> Don't use export files, put export the mexFunction explicitly.
# <AF> Consider replacing SHARED with MODULE
# add_library(${CPP_FILE} SHARED ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES} ${CMAKE_SOURCE_DIR}/Matlabdef.def)

add_library(${CPP_FILE} SHARED ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES}  ${CMAKE_SOURCE_DIR}/cmake_modules/Matlabdef.def )

# Add any extras dependencies
target_link_libraries(${CPP_FILE} rcl ril efl ${MATLAB_LIBRARIES})

# <AF> TODO: Move this conditional logic to a centralised loacation!
# 32-bit or 64-bit mex
if(WIN32)
  if (CMAKE_CL_64)
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexw64)
  else(CMAKE_CL_64)
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexw32)
  endif(CMAKE_CL_64)
else(WIN32)
  if (CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexa64 PREFIX "")
  else(CMAKE_SIZEOF_VOID_P MATCHES "8")
      SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX .mexglx PREFIX "")
  endif (CMAKE_SIZEOF_VOID_P MATCHES "8")
endif(WIN32)
# SET_TARGET_PROPERTIES(${CPP_FILE} PROPERTIES SUFFIX ${MATLAB_SUFFIX_MEX} )

# <AF> What to do with the install?
# install to /bin by default
# install(TARGETS ${CPP_FILE} DESTINATION ../bin)
