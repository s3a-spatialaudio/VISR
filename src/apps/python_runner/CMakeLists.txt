# Copyright Institute of Sound and Vibration Research - All rights reserved

set( APPLICATION_NAME python_runner )

ADD_EXECUTABLE( ${APPLICATION_NAME} main.cpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(${APPLICATION_NAME} audiointerfaces_static )
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} apputilities_static )
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} pythonsupport_static )
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} rrl_static )
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} pml_static )
TARGET_LINK_LIBRARIES(${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} )
