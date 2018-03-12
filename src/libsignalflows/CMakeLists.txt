# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES 
core_renderer.cpp
baseline_renderer.cpp 
bunch_renderer.cpp 
delay_vector.cpp 
gain_matrix.cpp
time_frequency_feedthrough.cpp
visr_renderer.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
baseline_renderer.hpp
bunch_renderer.hpp
core_renderer.hpp
delay_vector.hpp 
export_symbols.hpp
gain_matrix.hpp
time_frequency_feedthrough.hpp
visr_renderer.hpp
)

# Signal that Python support is activated
# It would be nicer if we could limit that to visr_renderer.cpp
if( BUILD_PYTHON_BINDINGS )
  add_definitions(  -DVISR_PYTHON_SUPPORT=1 )
endif( BUILD_PYTHON_BINDINGS )

add_library( signalflows_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( signalflows_static PROPERTIES OUTPUT_NAME signalflows )
# Transitive link dependencies
target_link_libraries( signalflows_static rcl_static )
target_link_libraries( signalflows_static pml_static )
target_link_libraries( signalflows_static reverbobject_static )

add_library( signalflows_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( signalflows_shared PROPERTIES OUTPUT_NAME signalflows )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( signalflows_shared PROPERTIES ARCHIVE_OUTPUT_NAME signalflows_shared )
target_compile_definitions( signalflows_shared PRIVATE -DVISR_BUILD_SIGNALFLOWS_SHARED_LIBRARY=1 -DVISR_BUILD_SIGNALFLOWS_LIBRARY=1)
target_compile_definitions( signalflows_shared PUBLIC -DVISR_BUILD_SIGNALFLOWS_SHARED_LIBRARY=1)
set_target_properties( signalflows_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
# Transitive link dependencies
target_link_libraries( signalflows_shared rcl_shared )
target_link_libraries( signalflows_shared reverbobject_shared )
target_link_libraries( signalflows_shared pml_shared )


# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( signalflows_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# If Python is activated, enable the optional use of a Metadapter to receive and process object metadata. 
if( BUILD_PYTHON_BINDINGS )
  # It would be nicer if we could limit that to visr_renderer.cpp
  add_definitions(  -DVISR_PYTHON_SUPPORT=1 )
  target_link_libraries( signalflows_static pythonsupport_static )
  target_link_libraries( signalflows_shared pythonsupport_shared )
endif( BUILD_PYTHON_BINDINGS )


install(TARGETS signalflows_static ARCHIVE DESTINATION lib )
install(TARGETS signalflows_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/visr/libsignalflows )


# Add the unit test directory.
ADD_SUBDIRECTORY( test )
