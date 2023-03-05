# Copyright Audioscenic 2023 - All rights reserved
# Author Andreas Franck <andreas.franck@audioscenic.com>

configure_file( pyproject.toml.in pyproject.toml )
configure_file( setup.cfg.in setup.cfg )
configure_file( MANIFEST.in.in MANIFEST.in )
configure_file( src/__init__.py src/visr/__init__.py )
configure_file( src/impl/__init__.py src/visr/impl/__init__.py )
configure_file( ${CMAKE_SOURCE_DIR}/Readme.md Readme.md )
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/visr/impl/ )


# TODO: Collect externals
set( EXTERNAL_TARGETS
 visrpython
 eflpython
 signalflowspython
 rbblpython
 rclpython
 rrlpython
 panningpython
 panningdsppython
 pmlpython
 objectmodelpython
 reverbobjectpython
 audiointerfacespython )

set( EXTERNAL_TARGET_FILES )
foreach( _external_target ${EXTERNAL_TARGETS} )
  list( APPEND EXTERNAL_TARGET_FILES $<TARGET_FILE:${_external_target}> )
endforeach()

add_custom_command( OUTPUT VISR-0.13.0-py37-windows.whl
  MAIN_DEPENDENCY pyproject.toml
  COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_TARGET_FILES} ${CMAKE_CURRENT_BINARY_DIR}/src/visr/impl/
  COMMAND ${PYTHON_EXECUTABLE} -m build --no-isolation --wheel
  DEPENDS visrpython
)

add_custom_target( visr_wheel DEPENDS VISR-0.13.0-py37-windows.whl )
