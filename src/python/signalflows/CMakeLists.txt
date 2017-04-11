# Python bindings for the main VISR interface classes

set( SOURCES
baseline_renderer.cpp
core_renderer.cpp
delay_vector.cpp
signalflows.cpp
)

set( HEADERS
baseline_renderer.hpp
core_renderer.hpp
delay_vector.hpp
)

set(PROJECT_NAME signalflowspython)

set(MODULE_NAME signalflows)


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


target_link_libraries( ${PROJECT_NAME} signalflows ) 

if( NOT BUILD_USE_PYBIND )
  # Standard dependencies for boost::python projects
  target_link_libraries(${PROJECT_NAME} ${Boost_PYTHON_LIBRARY} )
  target_link_libraries(${PROJECT_NAME} ${Boost_SYSTEM_LIBRARY} )
endif( NOT BUILD_USE_PYBIND )
target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARY} )

install(TARGETS ${PROJECT_NAME} DESTINATION ../python)
