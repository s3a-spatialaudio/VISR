# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES main.cpp init_filter_matrix.cpp options.cpp )
set( HEADERS init_filter_matrix.hpp options.hpp )

add_executable( matrix_convolver_app ${SOURCES} ${HEADERS} )

set_target_properties( matrix_convolver_app PROPERTIES OUTPUT_NAME matrix_convolver )

target_link_libraries(matrix_convolver_app apputilities_static )
target_link_libraries(matrix_convolver_app signalflows_static )
target_link_libraries(matrix_convolver_app rrl_static )
target_link_libraries(matrix_convolver_app audiointerfaces_static )