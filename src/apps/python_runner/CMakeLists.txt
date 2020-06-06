# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME python_runner )

add_executable( ${APPLICATION_NAME} main.cpp options.hpp options.cpp )

target_link_libraries(${APPLICATION_NAME} PRIVATE audiointerfaces_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE apputilities_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE pythoncomponents_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE pythonsupport_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE pml_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE rrl_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE visr_${BUILD_LIBRARY_TYPE_FOR_APPS} )
target_link_libraries(${APPLICATION_NAME} PRIVATE Boost::filesystem )

install( TARGETS ${APPLICATION_NAME} DESTINATION bin COMPONENT standalone_applications )
