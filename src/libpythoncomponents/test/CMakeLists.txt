# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME pythoncomponents_test )

set( SOURCES
test_main.cpp
wrapper.cpp
)

add_executable( ${APPLICATION_NAME} ${SOURCES} ) 

target_include_directories( ${APPLICATION_NAME} PRIVATE ${PYTHON_INCLUDE_DIR} )
# The pybind11 header is used to include Python.h.
# This avoids preprocessor magic to work around platform specifics.
target_include_directories( ${APPLICATION_NAME} PRIVATE ${PYBIND11_INCLUDE_DIR} )

target_link_libraries( ${APPLICATION_NAME} pythoncomponents_static )
target_link_libraries( ${APPLICATION_NAME} pythonsupport_static )
target_link_libraries( ${APPLICATION_NAME} rrl_static )
target_link_libraries( ${APPLICATION_NAME} Boost::filesystem )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
target_link_libraries( ${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )

# Since the 'wrapper' unit tests require the visr Python external, ensure that it is compiled
add_dependencies( ${APPLICATION_NAME} visrpython )

# Encode the current source location to give access to the Python code contained in the test/python directory.
target_compile_definitions( ${APPLICATION_NAME} PRIVATE CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

# Encode the path to the visr Python external into a compile definition
target_compile_definitions( ${APPLICATION_NAME} PRIVATE PYTHON_MODULE_INSTALL_DIRECTORY="$<TARGET_FILE_DIR:visrpython>" )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
