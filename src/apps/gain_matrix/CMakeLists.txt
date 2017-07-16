# Copyright Institute of Sound and Vibration Research - All rights reserved

ADD_EXECUTABLE( gain_matrix_app main.cpp options.cpp )

TARGET_LINK_LIBRARIES(gain_matrix_app signalflows)
TARGET_LINK_LIBRARIES(gain_matrix_app rcl)
TARGET_LINK_LIBRARIES(gain_matrix_app rbbl)
TARGET_LINK_LIBRARIES(gain_matrix_app rrl)
TARGET_LINK_LIBRARIES(gain_matrix_app audiointerfaces)
TARGET_LINK_LIBRARIES(gain_matrix_app visr_impl)
TARGET_LINK_LIBRARIES(gain_matrix_app pml)
TARGET_LINK_LIBRARIES(gain_matrix_app objectmodel) # newly pulled-in dependency of libpml
TARGET_LINK_LIBRARIES(gain_matrix_app efl)
TARGET_LINK_LIBRARIES(gain_matrix_app ${Boost_FILESYSTEM_LIBRARY} )
TARGET_LINK_LIBRARIES(gain_matrix_app ${Boost_PROGRAM_OPTIONS_LIBRARY} )
TARGET_LINK_LIBRARIES(gain_matrix_app ${Boost_SYSTEM_LIBRARY} )
