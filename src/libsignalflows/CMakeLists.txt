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

add_library( signalflows_static STATIC ${SOURCES} ${HEADERS} )
set_target_properties( signalflows_static PROPERTIES OUTPUT_NAME signalflows )
target_compile_definitions( signalflows_static PUBLIC VISR_SIGNALFLOWS_STATIC_LIBRARY=1)

add_library( signalflows_shared SHARED ${SOURCES} ${HEADERS} )
set_target_properties( signalflows_shared PROPERTIES OUTPUT_NAME signalflows )
# Specify a specific name for the .lib stub of the DLL on Windows to avoid clashes with the static library.
set_target_properties( signalflows_shared PROPERTIES ARCHIVE_OUTPUT_NAME signalflows_shared )
set_target_properties( signalflows_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib" )


# Set target properties common to both library types.
foreach(LIB_TYPE static shared)
  target_compile_definitions( signalflows_${LIB_TYPE} PRIVATE VISR_BUILD_SIGNALFLOWS_LIBRARY=1)
  target_link_libraries( signalflows_${LIB_TYPE} PUBLIC rcl_${LIB_TYPE} )
  target_link_libraries( signalflows_${LIB_TYPE} PUBLIC reverbobject_${LIB_TYPE} )
  target_link_libraries( signalflows_${LIB_TYPE} PUBLIC pml_${LIB_TYPE} )

  target_link_libraries( signalflows_${LIB_TYPE} PRIVATE Boost::system )
  if( NOT Boost_USE_STATIC_LIBS )
    target_compile_definitions( signalflows_${LIB_TYPE} PUBLIC -DBOOST_ALL_DYN_LINK )
  endif( NOT Boost_USE_STATIC_LIBS )
  target_link_libraries( signalflows_${LIB_TYPE} PUBLIC Boost::disable_autolinking )

  # If Python is activated, enable the optional use of a Metadapter to receive and process object metadata.
  if( BUILD_PYTHON_BINDINGS )
    set_property( SOURCE visr_renderer.cpp APPENDE PROPERTY COMPILE_DEFINITIONS VISR_PYTHON_SUPPORT=1 )
    target_link_libraries( signalflows_${LIB_TYPE} PRIVATE pythoncomponents_${LIB_TYPE} )
  endif( BUILD_PYTHON_BINDINGS )
endforeach()

# Set public headers to be installed for the shared target.
# Because the headers are the same for the static library, the headers are only
# installled once.
set_target_properties( signalflows_shared PROPERTIES PUBLIC_HEADER "${HEADERS}" )

# Set include paths for dependent projects
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
foreach(TARGET_TYPE static shared)
  target_include_directories(signalflows_${TARGET_TYPE}
    PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/visr>
  )
endforeach()

install(TARGETS signalflows_static
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT static_libraries )
install(TARGETS signalflows_shared
        EXPORT VISR_exports
        ARCHIVE DESTINATION lib COMPONENT development_files
        RUNTIME DESTINATION lib COMPONENT shared_libraries
        LIBRARY DESTINATION lib COMPONENT shared_libraries
        PUBLIC_HEADER DESTINATION include/visr/libsignalflows COMPONENT development_files )


# Add the unit test directory.
ADD_SUBDIRECTORY( test )
