# Copyright Institute of Sound and Vibration Research - All rights reserved

# Preliminary solution: communicate to main.cpp that Jack support is enabled.
# TODO: Replace by factory mechanism to select audio interface.
IF( BUILD_USE_JACK )
  ADD_DEFINITIONS( "-DVISR_JACK_SUPPORT" )
ENDIF( BUILD_USE_JACK )

SET( SOURCES main.cpp init_filter_matrix.cpp options.cpp )
SET( HEADERS init_filter_matrix.hpp options.hpp )

ADD_EXECUTABLE( matrix_convolver_app ${SOURCES} ${HEADERS} )

SET_TARGET_PROPERTIES( matrix_convolver_app PROPERTIES OUTPUT_NAME matrix_convolver )

TARGET_LINK_LIBRARIES(matrix_convolver_app apputilities)
TARGET_LINK_LIBRARIES(matrix_convolver_app signalflows)
TARGET_LINK_LIBRARIES(matrix_convolver_app rrl)
