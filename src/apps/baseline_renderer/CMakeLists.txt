# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( baseline_renderer main.cpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(baseline_renderer signalflows)
TARGET_LINK_LIBRARIES(baseline_renderer rrl)
TARGET_LINK_LIBRARIES(baseline_renderer visr_impl)
TARGET_LINK_LIBRARIES(baseline_renderer ${WHOLE_ARCHIVE_PREFIX} pml ${WHOLE_ARCHIVE_SUFFIX} )
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer ${CMAKE_THREAD_LIBS_INIT} )
