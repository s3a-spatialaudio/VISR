# Copyright Institute of Sound and Vibration Research - All rights reserved

set( SOURCES
wrapper.cpp
)

# Basically, this makes the files show up in the Visual studio project.
set( HEADERS
export_symbols.hpp
wrapper.hpp)

include_directories( ${PYTHON_INCLUDE_DIR} )
# The pybind11 header is used to include Python.h.
# This avoids preprocessor magic to work around platform specifics.
include_directories( ${PYBIND11_INCLUDE_DIR} )

add_library( pythoncomponents_static STATIC ${SOURCES} ${HEADERS} )
# Transitive dependencies
# Note: We always need to link the Pythonsupport shared library as a shared library, because there must be
# exactly one instance of pythonsupport::InitialisationGuard
target_link_libraries( pythoncomponents_static pythonsupport_shared )
target_link_libraries( pythoncomponents_static visr_static )

add_library( pythoncomponents_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( pythoncomponents_shared PROPERTIES OUTPUT_NAME pythoncomponents )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( pythoncomponents_shared PROPERTIES ARCHIVE_OUTPUT_NAME pythoncomponents_shared )
target_compile_definitions( pythoncomponents_shared PRIVATE -DVISR_BUILD_PYTHONCOMPONENTS_SHARED_LIBRARY=1 -DVISR_BUILD_PYTHONCOMPONENTS_LIBRARY=1)
target_compile_definitions( pythoncomponents_shared PUBLIC -DVISR_BUILD_PYTHONCOMPONENTS_SHARED_LIBRARY=1)
set_target_properties( pythoncomponents_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( pythoncomponents_shared pythonsupport_shared )
target_link_libraries( pythoncomponents_shared visr_shared )

install(TARGETS pythoncomponents_static ARCHIVE DESTINATION lib )
install(TARGETS pythoncomponents_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )

# Add the unit test directory.
add_subdirectory( test )
