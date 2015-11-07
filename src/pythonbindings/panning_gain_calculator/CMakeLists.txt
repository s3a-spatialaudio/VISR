# Compile a Python binding

# Configure your project 
set(CPP_FILE panning_gain_calculator)

set(PROJECT_NAME panning_gain_calculator)

# Add any project-specific CPP files here.
set( ADDITIONAL_CPP_FILES )


# Internal logic, do not edit.

include_directories(${PYTHON_INCLUDE_DIR})

set(CPP_FILE_NAME ${CPP_FILE}.cpp)

# Set the filename extension of Python external modules.
# TODO: This might be moved to a central location
if( WIN32 )
  set( PYTHON_EXTERNAL_EXTENSION ".pyd" )
ELSEIF( WIN32 )
  set( PYTHON_EXTERNAL_EXTENSION ${CMAKE_SHARED_LIBRARY_SUFFIX} )  
ENDIF( WIN32 )

add_library(${PROJECT_NAME} MODULE ${CPP_FILE_NAME} ${ADDITIONAL_CPP_FILES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CPP_FILE} )
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" )
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${PYTHON_EXTERNAL_EXTENSION} )
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )


# Add any extras dependencies
target_link_libraries(${PROJECT_NAME} panning pml efl )
target_link_libraries(${PROJECT_NAME} ${Boost_PYTHON_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
