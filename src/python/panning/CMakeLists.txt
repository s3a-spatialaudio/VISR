# Python bindings for the main VISR interface classes

set( SOURCES
loudspeaker_array.cpp
panning.cpp
vbap.cpp
xyz.cpp
)

set(PROJECT_NAME panningpython)

set(MODULE_NAME panning )

########################################################################
# Internal logic, do not edit.

include_directories(${PYTHON_INCLUDE_DIR})

# Set the filename extension of Python external modules if it differs from the 
# standard extensions for modules on this platform.
# TODO: This might be moved to a central location
if( WIN32 )
  set( PYTHON_EXTERNAL_SUFFIX ".pyd" )
endif( WIN32 )

add_library(${PROJECT_NAME} MODULE ${HEADERS} ${SOURCES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MODULE_NAME} )
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" )
if( PYTHON_EXTERNAL_SUFFIX )
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${PYTHON_EXTERNAL_SUFFIX} )
endif( PYTHON_EXTERNAL_SUFFIX )
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )

target_link_libraries( ${PROJECT_NAME} panning )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${Boost_REGEX_LIBRARY} )

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
