# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

ADD_EXECUTABLE( baseline_renderer_tracking main.cpp signal_flow.cpp signal_flow.hpp options.hpp options.cpp )

TARGET_LINK_LIBRARIES(baseline_renderer_tracking rcl)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking objectmodel)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking panning)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking pml)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking rbbl)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking ril)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking rrl)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking efl)
TARGET_LINK_LIBRARIES(baseline_renderer_tracking ${PORTAUDIO_LIBRARIES} )
TARGET_LINK_LIBRARIES(baseline_renderer_tracking ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer_tracking ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer_tracking ${Boost_THREAD_LIBRARY} )
TARGET_LINK_LIBRARIES(baseline_renderer_tracking ${Boost_PROGRAM_OPTIONS_LIBRARY} )
