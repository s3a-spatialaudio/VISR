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

add_library( pythonsupport_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( pythonsupport_static PROPERTIES OUTPUT_NAME pythonsupport )
target_compile_definitions( pythonsupport_static PUBLIC VISR_PYTHONSUPPORT_STATIC_LIBRARY=1)

add_library( pythonsupport_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( pythonsupport_shared PROPERTIES OUTPUT_NAME pythonsupport )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( pythonsupport_shared PROPERTIES ARCHIVE_OUTPUT_NAME pythonsupport_shared )
set_target_properties( pythonsupport_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )

# Set target properties common to both library types.
foreach(LIB_TYPE static shared)
  target_compile_definitions( pythonsupport_${LIB_TYPE} PRIVATE VISR_BUILD_PYTHONSUPPORT_LIBRARY=1)
  target_include_directories( pythonsupport_${LIB_TYPE} PRIVATE ${PYTHON_INCLUDE_DIR} )
  # The pybind11 header is used to include Python.h.
  # This avoids preprocessor magic to make this include statement
  # portable.
  target_include_directories( pythonsupport_${LIB_TYPE} PRIVATE ${PYBIND11_INCLUDE_DIR} )

  # Transitive dependencies
  target_link_libraries( pythonsupport_${LIB_TYPE} PRIVATE ${PYTHON_LIBRARY} )
  target_link_libraries( pythonsupport_${LIB_TYPE} PRIVATE Boost::filesystem )
  if( NOT Boost_USE_STATIC_LIBS )
    target_compile_definitions( pythonsupport_${LIB_TYPE} PRIVATE -DBOOST_ALL_DYN_LINK )
  endif( NOT Boost_USE_STATIC_LIBS )
  target_compile_definitions( pythonsupport_${LIB_TYPE} PRIVATE -DBOOST_ALL_NO_LIB )

  # Export the Python version as a compile definition.
  # This can be extracted by other projects importing this target.
  target_compile_definitions( pythonsupport_${LIB_TYPE} INTERFACE
                              VISR_PYTHON_VERSION=${PYTHONLIBS_VERSION_STRING} )
endforeach()

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( pythonsupport_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )


# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(pythonsupport_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

if( BUILD_INSTALL_STATIC_LIBRARIES )
  install(TARGETS pythonsupport_static
          EXPORT VISR_exports
          ARCHIVE DESTINATION lib COMPONENT static_libraries )
endif( BUILD_INSTALL_STATIC_LIBRARIES )
install(TARGETS pythonsupport_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/libpythonsupport COMPONENT development_files )
