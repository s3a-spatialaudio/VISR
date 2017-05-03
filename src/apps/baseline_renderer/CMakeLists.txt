# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_DEFINITIONS( -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

# Preliminary solution: communicate to main.cpp that Jack support is enabled.
# TODO: Replace by factory mechanism to select audio interface.
IF( BUILD_USE_JACK )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )
ENDIF( BUILD_USE_JACK )

ADD_EXECUTABLE( baseline_renderer main.cpp options.hpp options.cpp )

# TARGET_LINK_LIBRARIES(baseline_renderer apputilities)
TARGET_LINK_LIBRARIES(baseline_renderer signalflows)
TARGET_LINK_LIBRARIES(baseline_renderer rrl)
TARGET_LINK_LIBRARIES(baseline_renderer ${Boost_PROGRAM_OPTIONS_LIBRARY} )


INSTALL( TARGETS baseline_renderer DESTINATION bin )
