# Copyright Audioscenic 2023 - All rights reserved
# Author Andreas Franck <andreas.franck@audioscenic.com>

configure_file( pyproject.toml.in pyproject.toml )
configure_file( setup.py setup.py )
configure_file( setup.cfg.in setup.cfg )
configure_file( MANIFEST.in.in MANIFEST.in )
configure_file( src/__init__.py src/visr/__init__.py )
configure_file( src/audiointerfaces/__init__.py src/visr/audiointerfaces/__init__.py )
configure_file( src/efl/__init__.py src/visr/efl/__init__.py )
configure_file( src/objectmodel/__init__.py src/visr/objectmodel/__init__.py )
configure_file( src/panning/__init__.py src/visr/panning/__init__.py )
configure_file( src/panningdsp/__init__.py src/visr/panningdsp/__init__.py )
configure_file( src/pml/__init__.py src/visr/pml/__init__.py )
configure_file( src/rbbl/__init__.py src/visr/rbbl/__init__.py )
configure_file( src/reverbobject/__init__.py src/visr/reverbobject/__init__.py )
configure_file( src/rcl/__init__.py src/visr/rcl/__init__.py )
configure_file( src/rrl/__init__.py src/visr/rrl/__init__.py )
configure_file( src/signalflows/__init__.py src/visr/signalflows/__init__.py )
configure_file( src/impl/__init__.py src/visr/impl/__init__.py )
configure_file( ${CMAKE_SOURCE_DIR}/Readme.md Readme.md )

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
 audiointerfacespython
)

set( EXTERNAL_TARGET_FILES )
foreach( _external_target ${EXTERNAL_TARGETS} )
  list( APPEND EXTERNAL_TARGET_FILES $<TARGET_FILE:${_external_target}> )
endforeach()

include(GeneratePythonBinaryWheel)

# set( _WHEEL_FILE_NAME dummy.whl )
getBinaryPythonWheelName( visr 0.13.0 _WHEEL_FILE_NAME )
# set( _WHEEL_FILE_NAME dummy.whl )

message( STATUS "Wheel name: " ${_WHEEL_FILE_NAME} )
message( STATUS "EXTERNAL_TARGET_FILES: " ${EXTERNAL_TARGET_FILES} )

add_custom_command( OUTPUT visr-0.13.0-cp310-cp310-linux_x86_64.whl # ${_WHEEL_FILE_NAME}
  MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
  COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_TARGET_FILES} ${CMAKE_CURRENT_BINARY_DIR}/src/visr/impl/
  COMMAND ${PYTHON_EXECUTABLE} -m build --no-isolation --wheel
  DEPENDS ${EXTERNAL_TARGETS}
)

message( STATUS "Wheel name: " ${_WHEEL_FILE_NAME} )

add_custom_target( visr_wheel DEPENDS visr-0.13.0-cp310-cp310-linux_x86_64.whl ) # ${_WHEEL_FILE_NAME} )
