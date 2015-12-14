# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( gain_matrix_app main.cpp options.cpp signal_flow.cpp )

TARGET_LINK_LIBRARIES(gain_matrix_app rbbl)
TARGET_LINK_LIBRARIES(gain_matrix_app rcl)
TARGET_LINK_LIBRARIES(gain_matrix_app ril)
TARGET_LINK_LIBRARIES(gain_matrix_app rrl)
TARGET_LINK_LIBRARIES(gain_matrix_app pml)
TARGET_LINK_LIBRARIES(gain_matrix_app efl)
TARGET_LINK_LIBRARIES(gain_matrix_app ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(gain_matrix_app ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(gain_matrix_app ${Boost_SYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(gain_matrix_app ${SNDFILE_LIBRARY} ) # Dependency of libpml
TARGET_LINK_LIBRARIES(gain_matrix_app ${PORTAUDIO_LIBRARIES} )
IF( BUILD_USE_JACK )
  TARGET_LINK_LIBRARIES(gain_matrix_app ${JACK_LIBRARY} )
ENDIF( BUILD_USE_JACK )
