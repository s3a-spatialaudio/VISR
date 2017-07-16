# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( bunch_renderer main.cpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(bunch_renderer signalflows)
TARGET_LINK_LIBRARIES(bunch_renderer rcl)
TARGET_LINK_LIBRARIES(bunch_renderer objectmodel)
TARGET_LINK_LIBRARIES(bunch_renderer panning)
TARGET_LINK_LIBRARIES(bunch_renderer rbbl)
TARGET_LINK_LIBRARIES(bunch_renderer rrl)
TARGET_LINK_LIBRARIES(bunch_renderer audiointerfaces)
TARGET_LINK_LIBRARIES(bunch_renderer visr_impl)
TARGET_LINK_LIBRARIES(bunch_renderer pml)
TARGET_LINK_LIBRARIES(bunch_renderer efl)
TARGET_LINK_LIBRARIES(bunch_renderer ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(bunch_renderer ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(bunch_renderer ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(bunch_renderer ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(bunch_renderer ${CMAKE_THREAD_LIBS_INIT} )
