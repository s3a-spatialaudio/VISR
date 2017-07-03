# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES 
core_renderer.cpp
baseline_renderer.cpp 
bunch_renderer.cpp 
delay_vector.cpp 
gain_matrix.cpp
matrix_convolver.hpp
time_frequency_feedthrough.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
baseline_renderer.hpp
bunch_renderer.hpp
core_renderer.hpp
delay_vector.hpp 
export_symbols.hpp
gain_matrix.hpp
matrix_convolver.cpp
time_frequency_feedthrough.hpp
)

add_library( signalflows STATIC ${SOURCES} ${HEADERS} )
# Transitive link dependencies
target_link_libraries( signalflows rcl )
target_link_libraries( signalflows pml )
target_link_libraries( signalflows rsao )

add_library( signalflows_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( signalflows_shared PROPERTIES OUTPUT_NAME signalflows )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( signalflows_shared PROPERTIES ARCHIVE_OUTPUT_NAME signalflows_shared )
target_compile_definitions( signalflows_shared PRIVATE -DVISR_BUILD_SIGNALFLOWS_SHARED_LIBRARY=1 -DVISR_BUILD_SIGNALFLOWS_LIBRARY=1)
set_target_properties( signalflows_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive link dependencies
target_link_libraries( signalflows_shared rcl_shared )
target_link_libraries( signalflows_shared pml_shared )
target_link_libraries( signalflows_shared rsao_shared )


# Add the unit test directory.
ADD_SUBDIRECTORY( test )
