# Copyright Institute of Sound and Vibration Research - All rights reserved


SET( SOURCES
# hEADER-ONLY AT THE MMENT
)

SET( HEADERS
matrix_from_ndarray.hpp
vector_from_ndarray.hpp
)

add_library( pythonbindinghelpers INTERFACE )

# add_library( pythonbindinghelpers_shared SHARED ${SOURCES} ${HEADERS} )
# set_target_properties( pythonbindinghelpers_shared PROPERTIES OUTPUT_NAME pythonbindinghelpers )
# # Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
# set_target_properties( pythonbindinghelpers_shared PROPERTIES ARCHIVE_OUTPUT_NAME pythonbindinghelpers_shared )
# target_compile_definitions( pythonbindinghelpers_shared PRIVATE -DVISR_BUILD_PYTHONBINDINGHELPERS_SHARED_LIBRARY=1 -DVISR_BUILD_PYTHONBINDINGHELPERS_LIBRARY=1)
# target_compile_definitions( pythonbindinghelpers_shared PUBLIC -DVISR_BUILD_PYTHONBINDINGHELPERS_SHARED_LIBRARY=1)
# set_target_properties( pythonbindinghelpers_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# # Set public headers to be installed for the shared library target.
# # Because the headers are the same for the static library, the headers are only
# # installed once.
# set_target_properties( pythonbindinghelpers_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# install(TARGETS pythonbindinghelpers_static ARCHIVE DESTINATION lib )
# install(TARGETS pythonbindinghelpers_shared ARCHIVE DESTINATION lib RUNTIME DESTINATION lib LIBRARY DESTINATION lib  PUBLIC_HEADER DESTINATION include/visr/libpythonbindinghelpers )

# Add the unit test directory.
# ADD_SUBDIRECTORY( test )
