# Copyright Institute of Sound and Vibration Research - All rights reserved

SET( SOURCES
gil_ensure_guard.cpp
initialisation_guard.cpp
load_module.cpp
)

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS
export_symbols.hpp
gil_ensure_guard.hpp
initialisation_guard.hpp
load_module.hpp
)

INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_DIR} )

ADD_LIBRARY( pythonsupport_static STATIC ${SOURCES} ${HEADERS} )
# Transitive dependencies
target_link_libraries( pythonsupport_static PUBLIC ${PYTHON_LIBRARY} )
target_link_libraries( pythonsupport_static PUBLIC Boost::filesystem )
target_link_libraries( pythonsupport_static PUBLIC Boost::system )

add_library( pythonsupport_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( pythonsupport_shared PROPERTIES OUTPUT_NAME pythonsupport )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( pythonsupport_shared PROPERTIES ARCHIVE_OUTPUT_NAME pythonsupport_shared )
target_compile_definitions( pythonsupport_shared PRIVATE -DVISR_BUILD_PYTHONSUPPORT_SHARED_LIBRARY=1 -DVISR_BUILD_PYTHONSUPPORT_LIBRARY=1)
target_compile_definitions( pythonsupport_shared PUBLIC -DVISR_BUILD_PYTHONSUPPORT_SHARED_LIBRARY=1)
set_target_properties( pythonsupport_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )
target_link_libraries( pythonsupport_shared PUBLIC ${PYTHON_LIBRARY} )
target_link_libraries( pythonsupport_shared PUBLIC Boost::filesystem )
target_link_libraries( pythonsupport_shared PUBLIC Boost::system )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(pythonsupport_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS pythonsupport_static EXPORT VISR_exports ARCHIVE DESTINATION lib )
install(TARGETS pythonsupport_shared EXPORT VISR_exports ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib )

# Add the unit test directory.
# No unit test for this basic functionality yet.
# see libpythoncomponents/test
# ADD_SUBDIRECTORY( test )
