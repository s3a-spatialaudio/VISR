# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( convolver_matrix_app main.cpp )

TARGET_LINK_LIBRARIES(convolver_matrix_app signalflows)
TARGET_LINK_LIBRARIES(convolver_matrix_app rbbl)
TARGET_LINK_LIBRARIES(convolver_matrix_app rcl)
TARGET_LINK_LIBRARIES(convolver_matrix_app pml)
TARGET_LINK_LIBRARIES(convolver_matrix_app ril)
TARGET_LINK_LIBRARIES(convolver_matrix_app rrl)
TARGET_LINK_LIBRARIES(convolver_matrix_app efl)
TARGET_LINK_LIBRARIES(convolver_matrix_app ${PORTAUDIO_LIBRARIES} )

TARGET_LINK_LIBRARIES(convolver_matrix_app ${FFTS_LIBRARY} )
TARGET_LINK_LIBRARIES(convolver_matrix_app ${KISSFFT_LIBRARY} )
IF( BUILD_USE_FFTW )
  TARGET_LINK_LIBRARIES(convolver_matrix_app ${FFTW3_LIBRARY} )
  TARGET_LINK_LIBRARIES(convolver_matrix_app ${FFTW3F_LIBRARY} )
ENDIF( BUILD_USE_FFTW )
IF( BUILD_USE_IPP )
  TARGET_LINK_LIBRARIES(convolver_matrix_app  ${IPP_LIB_S} ${IPP_LIB_VM} ${IPP_LIB_CORE} )
ENDIF( BUILD_USE_IPP )
