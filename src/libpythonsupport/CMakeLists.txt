# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
initialisation_guard.cpp
python_wrapper.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
export_symbols.hpp
initialisation_guard.hpp
python_wrapper.hpp
)

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

ADD_LIBRARY( pythonsupport_static STATIC ${SOURCES} ${HEADERS} )
# Transitive dependencies
target_link_libraries( pythonsupport_static visr_static )
target_link_libraries( pythonsupport_static ${PYTHON_LIBRARY} )
target_link_libraries( pythonsupport_static ${Boost_FILESYSTEM_LIBRARY} )
target_link_libraries( pythonsupport_static ${Boost_SYSTEM_LIBRARY} )

add_library( pythonsupport_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( pythonsupport_shared PROPERTIES OUTPUT_NAME pythonsupport )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( pythonsupport_shared PROPERTIES ARCHIVE_OUTPUT_NAME pythonsupport_shared )
target_compile_definitions( pythonsupport_shared PRIVATE -DVISR_BUILD_PYTHONSUPPORT_SHARED_LIBRARY=1 -DVISR_BUILD_PYTHONSUPPORT_LIBRARY=1)
target_compile_definitions( pythonsupport_shared PUBLIC -DVISR_BUILD_PYTHONSUPPORT_SHARED_LIBRARY=1)
set_target_properties( pythonsupport_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

install(TARGETS pythonsupport_static ARCHIVE DESTINATION lib )
install(TARGETS pythonsupport_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
