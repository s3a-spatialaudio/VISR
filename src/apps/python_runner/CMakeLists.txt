# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME python_runner )

add_executable( ${APPLICATION_NAME} main.cpp options.hpp options.cpp )

target_link_libraries(${APPLICATION_NAME} PRIVATE audiointerfaces_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE apputilities_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE pythoncomponents_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE pythonsupport_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE pml_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE rrl_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE visr_shared )
target_link_libraries(${APPLICATION_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT} )

install( TARGETS ${APPLICATION_NAME} DESTINATION bin COMPONENT standalone_applications )
