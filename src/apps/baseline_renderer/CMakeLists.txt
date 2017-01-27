# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( baseline_renderer main.cpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(baseline_renderer signalflows)
TARGET_LINK_LIBRARIES(baseline_renderer rcl)
TARGET_LINK_LIBRARIES(baseline_renderer objectmodel)
TARGET_LINK_LIBRARIES(baseline_renderer panning)
TARGET_LINK_LIBRARIES(baseline_renderer rbbl)
TARGET_LINK_LIBRARIES(baseline_renderer rrl)
TARGET_LINK_LIBRARIES(baseline_renderer visr_impl)
TARGET_LINK_LIBRARIES(baseline_renderer -Wl,--whole-archive pml -Wl,--no-whole-archive)
TARGET_LINK_LIBRARIES(baseline_renderer efl)
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer ${CMAKE_THREAD_LIBS_INIT} )
