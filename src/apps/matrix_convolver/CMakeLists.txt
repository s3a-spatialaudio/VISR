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
TARGET_LINK_LIBRARIES(matrix_convolver_app rcl)
TARGET_LINK_LIBRARIES(matrix_convolver_app rbbl)
TARGET_LINK_LIBRARIES(matrix_convolver_app pml)
TARGET_LINK_LIBRARIES(matrix_convolver_app ril)
TARGET_LINK_LIBRARIES(matrix_convolver_app rrl)
TARGET_LINK_LIBRARIES(matrix_convolver_app efl)
TARGET_LINK_LIBRARIES(matrix_convolver_app ${PORTAUDIO_LIBRARIES} )

IF( BUILD_USE_JACK )
  TARGET_LINK_LIBRARIES(matrix_convolver_app ${JACK_LIBRARY} )
ENDIF( BUILD_USE_JACK )

TARGET_LINK_LIBRARIES(matrix_convolver_app ${FFTS_LIBRARY} )
TARGET_LINK_LIBRARIES(matrix_convolver_app ${KISSFFT_LIBRARY} )
IF( BUILD_USE_FFTW )
  TARGET_LINK_LIBRARIES(matrix_convolver_app ${FFTW3_LIBRARY} )
  TARGET_LINK_LIBRARIES(matrix_convolver_app ${FFTW3F_LIBRARY} )
ENDIF( BUILD_USE_FFTW )
IF( BUILD_USE_IPP )
  TARGET_LINK_LIBRARIES(matrix_convolver_app  ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
ENDIF( BUILD_USE_IPP )
TARGET_LINK_LIBRARIES(matrix_convolver_app ${SNDFILE_LIBRARY} )
TARGET_LINK_LIBRARIES(matrix_convolver_app ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(matrix_convolver_app ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(matrix_convolver_app ${Boost_PROGRAM_OPTIONS_LIBRARY} )

INSTALL( TARGETS matrix_convolver_app DESTINATION bin )
