# Compile a Python binding

# User configuration section
set(CPP_FILE panning_gain_calculator)

set(PROJECT_NAME panning_gain_calculator)

# Add any project-specific CPP files here.
set( ADDITIONAL_CPP_FILES )

# Added any required additional libraries
set( ADDITIONAL_LIBRARIES  panning pml efl ${Boost_FILESYSTEM_LIBRARY} )

########################################################################
# Internal logic, do not edit.

# Trigger separate handling dependent on whether the boost libraries
# are linked statically or dynamically
IF( Boost_USE_STATIC_LIBS )
  ADD_DEFINITIONS( -DBOOST_USE_STATIC_LIBS=1 )
ELSEIF( Boost_USE_STATIC_LIBS )
  ADD_DEFINITIONS( -DBOOST_USE_STATIC_LIBS=0 )
ENDIF( Boost_USE_STATIC_LIBS )

include_directories(${PYTHON_INCLUDE_DIR})

set(CPP_FILE_NAME ${CPP_FILE}.cpp)

# Set the filename extension of Python external modules if it differs from the 
# standard extensions for modules on this platform.
# TODO: This might be moved to a central location
if( WIN32 )
  set( PYTHON_EXTERNAL_SUFFIX ".pyd" )
endif( WIN32 )

add_library(${PROJECT_NAME} MODULE ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} )
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" )
if( PYTHON_EXTERNAL_SUFFIX )
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${PYTHON_EXTERNAL_SUFFIX} )
endif( PYTHON_EXTERNAL_SUFFIX )
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )

target_link_libraries(${PROJECT_NAME} ${ADDITIONAL_LIBRARIES} )
# Standard dependencies for boost::python projects
target_link_libraries(${PROJECT_NAME} ${Boost_PYTHON_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
