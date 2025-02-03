# Copyright (c) 2014-2023 Andreas Franck andreasfranckilmenau@googlemail.com --- All rights reserved.

# Function to define the installation of a library with the following features:
# * Takes care of the different library variants (shared, static, static_pic) that
#   are build depending on the respective CMake define.
# * Specify runtime dependencies to be installed with the package.
# Parameters:
# baseName: base name of the library target, will be suffixed with _shared, _static_,
#   or _static_pic for the different library variants.
function( installLibraryWithDependencies baseName )
  if( BUILD_INSTALL_STATIC_LIBRARIES )
    install(TARGETS ${baseName}_static
      EXPORT VISR_static_library_exports
      PUBLIC_HEADER DESTINATION include/visr/lib${baseName} COMPONENT development_files
      FRAMEWORK DESTINATION frameworks COMPONENT static_libraries
      ARCHIVE DESTINATION lib COMPONENT static_libraries)
  endif( BUILD_INSTALL_STATIC_LIBRARIES )

  if( BUILD_INSTALL_SHARED_LIBRARIES )
    install(TARGETS ${baseName}_shared
            RUNTIME_DEPENDENCIES
              PRE_EXCLUDE_REGEXES ${VISR_RUNTIME_DEPENDENCIES_PRE_EXCLUDE_REGEXES}
              POST_EXCLUDE_REGEXES ${VISR_RUNTIME_DEPENDENCIES_POST_EXCLUDE_REGEXES}
              DIRECTORIES ${VISR_RUNTIME_DEPENDENCIES_SEARCH_PATH}
            EXPORT VISR_shared_library_exports
            ARCHIVE DESTINATION lib COMPONENT development_files
            RUNTIME DESTINATION bin COMPONENT shared_libraries
            FRAMEWORK DESTINATION frameworks COMPONENT shared_libraries
            LIBRARY DESTINATION lib  COMPONENT shared_libraries
            PUBLIC_HEADER DESTINATION include/visr/lib${baseName} COMPONENT development_files )
  endif( BUILD_INSTALL_SHARED_LIBRARIES )

  if( BUILD_INSTALL_STATIC_PIC_LIBRARIES )
    install(TARGETS ${baseName}_static_pic
            EXPORT VISR_static_pic_library_exports
            PUBLIC_HEADER DESTINATION include/visr/lib${baseName} COMPONENT development_files
            ARCHIVE DESTINATION lib COMPONENT static_pic_libraries
            FRAMEWORK DESTINATION frameworks COMPONENT static_pic_libraries )
  endif( BUILD_INSTALL_STATIC_PIC_LIBRARIES )
endfunction()

# Install a shared library into the Python package directory.
# This is conditional on both Python bindings and shared library variants being built,
# otherwise this is a no-op.
# Parameters:
# baseName The name of the library target w/o the 'shared' suffix.
function( installSharedLibraryInPythonPackage baseName )
  if( BUILD_INSTALL_SHARED_LIBRARIES AND BUILD_PYTHON_BINDINGS )
    install(TARGETS ${baseName}_shared
            RUNTIME_DEPENDENCIES
              PRE_EXCLUDE_REGEXES ${VISR_RUNTIME_DEPENDENCIES_PRE_EXCLUDE_REGEXES}
              POST_EXCLUDE_REGEXES ${VISR_RUNTIME_DEPENDENCIES_POST_EXCLUDE_REGEXES}
              DIRECTORIES ${VISR_RUNTIME_DEPENDENCIES_SEARCH_PATH}
            # EXPORT VISR_shared_library_exports
            ARCHIVE DESTINATION ${PYTHON_EXTERNAL_INSTALL_DIRECTORY} COMPONENT python_externals
            RUNTIME DESTINATION ${PYTHON_EXTERNAL_INSTALL_DIRECTORY} COMPONENT python_externals
            FRAMEWORK DESTINATION ${PYTHON_EXTERNAL_INSTALL_DIRECTORY} COMPONENT python_externals
            LIBRARY DESTINATION ${PYTHON_EXTERNAL_INSTALL_DIRECTORY}  COMPONENT python_externals
            PUBLIC_HEADER DESTINATION include/visr/lib${baseName} COMPONENT python_externals )
  endif()
endfunction()

function( installExecutableWithDependencies targetName )
  install(TARGETS ${targetName}
          RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES ${VISR_RUNTIME_DEPENDENCIES_PRE_EXCLUDE_REGEXES}
            POST_EXCLUDE_REGEXES ${VISR_RUNTIME_DEPENDENCIES_POST_EXCLUDE_REGEXES}
            DIRECTORIES ${RUNTIME_DEPENDENCIES_SEARCH_PATH}
          EXPORT VISR_static_library_exports
          COMPONENT standalone_applications )
endfunction()
