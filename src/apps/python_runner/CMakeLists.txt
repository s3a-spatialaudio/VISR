# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME python_runner )

add_executable( ${APPLICATION_NAME} main.cpp options.hpp options.cpp )

target_link_libraries(${APPLICATION_NAME} PRIVATE audiointerfaces_static )
target_link_libraries(${APPLICATION_NAME} PRIVATE apputilities_static )
target_link_libraries(${APPLICATION_NAME} PRIVATE pythoncomponents_static )
target_link_libraries(${APPLICATION_NAME} PRIVATE pythonsupport_static )
target_link_libraries(${APPLICATION_NAME} PRIVATE pml_static )
target_link_libraries(${APPLICATION_NAME} PRIVATE rrl_static )
target_link_libraries(${APPLICATION_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT} )

install( TARGETS ${APPLICATION_NAME} DESTINATION bin COMPONENT standalone_applications )
