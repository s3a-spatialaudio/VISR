# Python bindings for the main VISR interface classes

set( SOURCES
baseline_renderer.cpp
core_renderer.cpp
delay_vector.cpp
signalflows.cpp
visr_renderer.cpp
)

set( HEADERS
baseline_renderer.hpp
core_renderer.hpp
delay_vector.hpp
)

set(PROJECT_NAME signalflowspython)

set(MODULE_NAME signalflows)

pybind11_add_module(${PROJECT_NAME} MODULE ${HEADERS} ${SOURCES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MODULE_NAME} )

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ${PYTHON_MODULE_EXTENSION} )

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )

# In case of Windows, we need to make sure that the import library part does not conflict
# with a static library of the same name.
# So we leave it in the build directory and add an _import suffix.
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_NAME "${PROJECT_NAME}_import" )

target_link_libraries( ${PROJECT_NAME} PRIVATE signalflows_shared ) 

install(TARGETS ${PROJECT_NAME} DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY})
