# Python bindings for the standard VISR interface component

set( SOURCES
biquad_coefficient.cpp
core_convolver_uniform.cpp
crossfading_convolver_uniform.cpp
filter_routing.cpp
fractional_delay_base.cpp
fractional_delay_factory.cpp
interpolating_convolver_uniform.cpp
interpolation_parameter.cpp
lagrange_interpolator.cpp
multichannel_convolver_uniform.cpp
object_channel_allocator.cpp
parametric_iir_coefficient.cpp
parametric_iir_coefficient_calculator.cpp
position_3d.cpp
quaternion.cpp
sparse_gain_routing.cpp
rbbl.cpp
)

# We don't need header files

set(PROJECT_NAME rbblpython)

set(MODULE_NAME rbbl)

pybind11_add_module(${PROJECT_NAME} MODULE ${HEADERS} ${SOURCES} )

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${MODULE_NAME} )

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/python" )

# In case of Windows, we need to make sure that the import library part does not conflict
# with a static library of the same name.
# So we leave it in the build directory and add an _import suffix.
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
set_target_properties( ${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_NAME "${PROJECT_NAME}_import" )

target_link_libraries( ${PROJECT_NAME} PRIVATE rbbl_shared )

set_target_properties( ${PROJECT_NAME} PROPERTIES FOLDER python_externals )

# Note: We export the target to enable dependent projects to query target properties
# such as the library location.
install(TARGETS ${PROJECT_NAME}
        EXPORT VISR_python_externals_exports
        DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY}
        COMPONENT python_externals )

if( BUILD_TESTING )
  add_subdirectory( test )
endif( BUILD_TESTING )
