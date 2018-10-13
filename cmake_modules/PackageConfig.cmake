# Copyright Institute of Sound and Vibration Research - All rights reserved

# Package generation configuration

SET( CPACK_DEBIAN_PACKAGE_MAINTAINER "Andreas Franck A.Franck@soton.ac.uk" )
SET( CPACK_PACKAGE_NAME "VISR" )
SET( CPACK_PACKAGE_VENDOR "ISVR")

# Use the version numbers centrally defined for the VISR project
set( CPACK_PACKAGE_VERSION_MAJOR ${VISR_MAJOR_VERSION} )
set( CPACK_PACKAGE_VERSION_MINOR ${VISR_MINOR_VERSION} )
set( CPACK_PACKAGE_VERSION_PATCH ${VISR_PATCH_VERSION} )

set( PKG_FILE_NAME ${VISR_VERSIONED_NAME}-${CMAKE_SYSTEM_NAME})

# Encode the Python major/minor version in the package file name if Python is enabled.
if( BUILD_PYTHON_BINDINGS )
  string( REGEX MATCH "[0-9]+.[0-9]+" PYTHON_MAJOR_MINOR ${PYTHONLIBS_VERSION_STRING} )
  set( PKG_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-python${PYTHON_MAJOR_MINOR}-${CMAKE_SYSTEM_NAME}" )
endif( BUILD_PYTHON_BINDINGS )

set( CPACK_PACKAGE_FILE_NAME ${PKG_FILE_NAME} )
SET( CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

if( WIN32 )
  set( CPACK_GENERATOR NSIS )
  set( CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
  get_filename_component( PORTAUDIO_LIBRARY_DIR ${PORTAUDIO_LIBRARY} DIRECTORY )
  install( FILES ${PORTAUDIO_LIBRARY_DIR}/portaudio_x64.dll DESTINATION 3rd COMPONENT thirdparty_libraries )

  get_filename_component( SNDFILE_LIBRARY_DIR ${SNDFILE_LIBRARY} DIRECTORY )
  install( FILES ${SNDFILE_LIBRARY_DIR}/libsndfile-1.dll DESTINATION 3rd COMPONENT thirdparty_libraries )

  # Boost
  if( NOT Boost_USE_STATIC_LIBS )
    foreach( BOOSTLIB ${VISR_BOOST_LIBRARIES} )
      get_target_property( BOOSTLIBPATH Boost::${BOOSTLIB} IMPORTED_LOCATION_RELEASE )
      get_filename_component( BOOSTLIBNAME ${BOOSTLIBPATH} NAME_WE )
      get_filename_component( BOOSTLIBDIR ${BOOSTLIBPATH} DIRECTORY )
      install( FILES ${BOOSTLIBDIR}/${BOOSTLIBNAME}.dll DESTINATION 3rd COMPONENT thirdparty_libraries)
    endforeach()
  endif( NOT Boost_USE_STATIC_LIBS )
endif( WIN32 )

if( VISR_SYSTEM_NAME MATCHES "Linux" )
  set( CPACK_GENERATOR DEB TBZ2 )
  set( CPACK_DEBIAN_HOMEPAGE "http://www.s3a-spatialaudio.org" )
  set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif(VISR_SYSTEM_NAME MATCHES "Linux")

if( VISR_SYSTEM_NAME MATCHES "MacOS" )
  set( CPACK_GENERATOR productbuild )

  set( CPACK_PACKAGE_NAME ${PKG_FILE_NAME}.pkg )

  set( CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )

  install( FILES ${PORTAUDIO_LIBRARIES} DESTINATION 3rd COMPONENT thirdparty_libraries)
  install( FILES ${SNDFILE_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)  
  install( FILES ${FLAC_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)
  install( FILES ${OGG_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)
  install( FILES ${VORBIS_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)
  install( FILES ${VORBISENC_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)

  if( NOT Boost_USE_STATIC_LIBS )
    foreach( BOOSTLIB ${VISR_BOOST_LIBRARIES} )
      get_target_property( BOOSTLIBPATH Boost::${BOOSTLIB} IMPORTED_LOCATION )
      install( FILES ${BOOSTLIBPATH} DESTINATION 3rd COMPONENT thirdparty_libraries)
    endforeach()
  endif( NOT Boost_USE_STATIC_LIBS )

  configure_file(${CMAKE_SOURCE_DIR}/cmake_modules/package_resources/productbuild_postscript.sh.in
  		       "${PROJECT_BINARY_DIR}/package_resources/productbuild_postscript.sh" @ONLY)
  set(CPACK_POSTFLIGHT_SHARED_LIBRARIES_SCRIPT ${PROJECT_BINARY_DIR}/package_resources/productbuild_postscript.sh)   

endif( VISR_SYSTEM_NAME MATCHES "MacOS" )

install( DIRECTORY config DESTINATION ${VISR_TOPLEVEL_INSTALL_DIRECTORY} COMPONENT loudspeaker_configs )
install( FILES ${CMAKE_SOURCE_DIR}/LICENSE.md
               ${CMAKE_SOURCE_DIR}/Contributors.md
               ${CMAKE_SOURCE_DIR}/Readme.md
               ${CMAKE_SOURCE_DIR}/ChangeLog.txt
               DESTINATION ${VISR_TOPLEVEL_INSTALL_DIRECTORY} COMPONENT base )
# Allow for text substitution in the files and change names from .md to .txt (the former is not currently supported by CMake).
configure_file (${CMAKE_SOURCE_DIR}/LICENSE.md
        "${PROJECT_BINARY_DIR}/package_resources/LICENSE.txt" @ONLY)
set( CPACK_RESOURCE_FILE_LICENSE ${PROJECT_BINARY_DIR}/package_resources/LICENSE.txt )
configure_file (${CMAKE_SOURCE_DIR}/Readme.md
        "${PROJECT_BINARY_DIR}/package_resources/Readme.txt" @ONLY)
set( CPACK_RESOURCE_FILE_README ${PROJECT_BINARY_DIR}/package_resources/Readme.txt )
configure_file (${CMAKE_SOURCE_DIR}/cmake_modules/package_resources/welcome.txt.in
        "${PROJECT_BINARY_DIR}/package_resources/welcome.txt" @ONLY)
set(CPACK_RESOURCE_FILE_WELCOME ${PROJECT_BINARY_DIR}/package_resources/welcome.txt)


# Install Python example scripts and templates
install( DIRECTORY src/python/templates DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY} COMPONENT python_templates )
install( DIRECTORY src/python/packages/visr_bst DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY} COMPONENT python_package_bst )
install( DIRECTORY src/python/packages/metadapter DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY} COMPONENT python_package_metadapter )

# CPack must be included after all install directives and CPACK_ variable definitions.
include( CPack )

# Define components and component groups for the component-enabled installers.
# This must happen after include( CPack )

# Installation types are ATM only supported by NSIS.
cpack_add_install_type( default DISPLAY_NAME "Default" )
cpack_add_install_type( developer DISPLAY_NAME "Developer" )
cpack_add_install_type( full DISPLAY_NAME "Full" )
cpack_add_install_type( python DISPLAY_NAME "Python developer" )

cpack_add_component( base
                     DISPLAY_NAME "Base"
                     DESCRIPTION "Basic required files."
                     REQUIRED HIDDEN
                   )

cpack_add_component( thirdparty_libraries
                     DISPLAY_NAME "Third-party libraries"
                     DESCRIPTION "Shared 3rd-party libraries."
                     REQUIRED HIDDEN
                   )

cpack_add_component(shared_libraries
                    DISPLAY_NAME "Shared Libraries"
                    DESCRIPTION "Core VISR libraries (shared)"
                    REQUIRED
                   )

if( BUILD_INSTALL_STATIC_LIBRARIES )
  cpack_add_component(static_libraries
                      DISPLAY_NAME "Static Libraries"
                      DESCRIPTION "Core VISR libraries (static)"
                      INSTALL_TYPES full developer
                      DISABLED
                     )
endif( BUILD_INSTALL_STATIC_LIBRARIES )

cpack_add_component( standalone_applications
                    DISPLAY_NAME "Standalone applications"
                    DESCRIPTION "Standalone command-line applications"
                    INSTALL_TYPES default full
                    DEPENDS shared_libraries
                   )

cpack_add_component( standalone_applications
                    DISPLAY_NAME "Standalone applications"
                    DESCRIPTION "Standalone command-line applications"
                    INSTALL_TYPES default full
                    DEPENDS shared_libraries
                   )

if( BUILD_INSTALL_STATIC_LIBRARIES )
  cpack_add_component(development_files
                      DISPLAY_NAME "Development files"
                      DESCRIPTION "Header files and CMake support"
                      INSTALL_TYPES developer full
                      DEPENDS static_libraries
                      DISABLED # Not contained in standard installation
                     )
else( BUILD_INSTALL_STATIC_LIBRARIES )
  cpack_add_component(development_files
                      DISPLAY_NAME "Development files"
                      DESCRIPTION "Header files and CMake support"
                      INSTALL_TYPES developer full
                      DISABLED # Not contained in standard installation
                     )
endif( BUILD_INSTALL_STATIC_LIBRARIES )

cpack_add_component( python_externals
                    DISPLAY_NAME "Python Externals"
                    DESCRIPTION "Python modules to access the VISR functionality from Python"
                    INSTALL_TYPES default python full
                   )

cpack_add_component_group( python_packages
                          DISPLAY_NAME "Python packages"
                          DESCRIPTION "VISR extension packages implemented in Python"
                          )

cpack_add_component( python_package_bst
                    DISPLAY_NAME "Binaural Synthesis Toolkit"
                    DESCRIPTION "Toolbox for binaural synthesis"
                    INSTALL_TYPES default python full
                    DEPENDS python_externals
                    GROUP python_packages
                   )

cpack_add_component( python_package_metadapter
                    DISPLAY_NAME "Metadapter"
                    DESCRIPTION "Extensible system for object metadata adaptation"
                    INSTALL_TYPES default python full
                    DEPENDS python_externals
                    GROUP python_packages
                   )

cpack_add_component( python_templates
                    DISPLAY_NAME "Python Templates"
                    DESCRIPTION "Python template files"
                    DEPENDS python_externals 
                    INSTALL_TYPES python full
                   )

cpack_add_component_group( documentation
                          DISPLAY_NAME "Documentation"
                          DESCRIPTION "User and API documentation"
                          )

cpack_add_component( user_documentation_pdf
                    DISPLAY_NAME "User documentation (PDF)"
                    DESCRIPTION "User documentation"
                    INSTALL_TYPES default developer full
                    GROUP documentation
                   )

cpack_add_component( api_documentation_pdf
                    DISPLAY_NAME "API documentation (PDF)"
                    DESCRIPTION "Code reference documentation"
                    INSTALL_TYPES developer full
                    GROUP documentation
                   )

cpack_add_component( doxygen_documentation_html
                    DISPLAY_NAME "Doxygen code documentation"
                    DESCRIPTION "Code reference documentation in Doxygen format (deprecated)"
                    INSTALL_TYPES full
                    DISABLED
                    GROUP documentation
                   )

cpack_add_component( loudspeaker_configs
                    DISPLAY_NAME "Loudspeaker configuration files"
                    DESCRIPTION "Example configuration files for the loudspeaker renderers."
                    INSTALL_TYPES default developer full
                   )
