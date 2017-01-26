# compile mex

# Code adapted from
# http://www.mathworks.com/matlabcentral/fileexchange/45522-mex-cmake

set(PROJECT_NAME time_frequency_feedthrough_mex)

set(CPP_FILE time_frequency_feedthrough)

# set up matlab libraries
set(CPP_FILE_NAME ${CPP_FILE}.cpp)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

# <AF> Don't use export files, put export the mexFunction explicitly.
# <AF> Consider replacing SHARED with MODULE
add_library(${PROJECT_NAME} SHARED ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES}  ${CMAKE_SOURCE_DIR}/cmake_modules/Matlabdef.def )

# Add any extras dependencies
target_link_libraries( ${PROJECT_NAME} signalflows )
target_link_libraries( ${PROJECT_NAME} rcl )
target_link_libraries( ${PROJECT_NAME} rbbl )
target_link_libraries( ${PROJECT_NAME} visr_impl )
target_link_libraries( ${PROJECT_NAME} pml )
target_link_libraries( ${PROJECT_NAME} objectmodel )
target_link_libraries( ${PROJECT_NAME} rrl )
target_link_libraries( ${PROJECT_NAME} efl )
target_link_libraries( ${PROJECT_NAME} mexsupport )
target_link_libraries( ${PROJECT_NAME} ${MATLAB_LIBRARIES} )
target_link_libraries( ${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( ${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} )

# Set the name of the mex object (and thus the Matlab function to be called) independently
# of the project name.
# Also set the file extension according to the operating system.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} SUFFIX ${MATLAB_MEX_SUFFIX} PREFIX "")

# Put directly into the mex/ subdirectory
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/mex" )

# <AF> What to do with the install?
# install to /bin by default
# install(TARGETS ${PROJECT_NAME} DESTINATION ../bin)
