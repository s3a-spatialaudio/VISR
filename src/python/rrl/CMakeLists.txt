# Python bindings for the main VISR interface classes

set( SOURCES
audio_signal_flow.cpp
rrl.cpp
)

set( HEADERS
audio_signal_flow.hpp
)


# We don't need header files

set(PROJECT_NAME rrlpython)

set(MODULE_NAME rrl)


########################################################################
# Internal logic, do not edit.

# Trigger separate handling dependent on whether the boost libraries
# are linked statically or dynamically
IF( Boost_USE_STATIC_LIBS )
  ADD_DEFINITIONS( -DBOOST_PYTHON_STATIC_LIBS=1 )
ENDIF( Boost_USE_STATIC_LIBS )

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

# Standard dependencies for boost::python projects
target_link_libraries( ${PROJECT_NAME} rrl )
target_link_libraries( ${PROJECT_NAME} pml ) # Preliminary solution (vulgo 'hack')
target_link_libraries(${PROJECT_NAME} ${Boost_PYTHON_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} )
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
