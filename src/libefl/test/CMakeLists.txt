# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME efl_test )

add_executable( ${APPLICATION_NAME} test_main.cpp complex_multiply.cpp lagrange_interpolator.cpp )

target_link_libraries( ${APPLICATION_NAME} rbbl_static )
target_link_libraries( ${APPLICATION_NAME} efl_static )
target_link_libraries( ${APPLICATION_NAME} Boost::unit_test_framework )
if( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions(${APPLICATION_NAME} PRIVATE -DBOOST_ALL_DYN_LINK )
endif( NOT Boost_USE_STATIC_LIBS )
target_compile_definitions( ${APPLICATION_NAME} PRIVATE -DBOOST_ALL_NO_LIB )

set_target_properties( ${APPLICATION_NAME} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_binaries)

add_test(NAME ${APPLICATION_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
         COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_binaries/${APPLICATION_NAME} )
