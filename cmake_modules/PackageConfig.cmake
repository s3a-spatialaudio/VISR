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

IF( WIN32 )
SET( CPACK_GENERATOR NSIS ZIP )
SET(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
GET_FILENAME_COMPONENT( PORTAUDIO_LIBRARY_DIR ${PORTAUDIO_LIBRARY} DIRECTORY )
INSTALL( FILES ${PORTAUDIO_LIBRARY_DIR}/portaudio_x64.dll DESTINATION 3rd)

GET_FILENAME_COMPONENT( SNDFILE_LIBRARY_DIR ${SNDFILE_LIBRARY} DIRECTORY )
INSTALL( FILES ${SNDFILE_LIBRARY_DIR}/libsndfile-1.dll DESTINATION 3rd)

# only valid in NSIS
# set(CPACK_ALL_INSTALL_TYPES Full Developer)

# Boost
  GET_FILENAME_COMPONENT( BOOST_CHRONO_LIBRARY_BASENAME ${Boost_CHRONO_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_DATE_TIME_LIBRARY_BASENAME ${Boost_DATE_TIME_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_FILESYSTEM_LIBRARY_BASENAME ${Boost_FILESYSTEM_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_PROGRAM_OPTIONS_LIBRARY_BASENAME ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_SYSTEM_LIBRARY_BASENAME ${Boost_SYSTEM_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_THREAD_LIBRARY_BASENAME ${Boost_THREAD_LIBRARY_RELEASE} NAME_WE )
  GET_FILENAME_COMPONENT( BOOST_REGEX_LIBRARY_BASENAME ${Boost_REGEX_LIBRARY_RELEASE} NAME_WE )
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_CHRONO_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_DATE_TIME_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_FILESYSTEM_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_PROGRAM_OPTIONS_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_SYSTEM_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_THREAD_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_CHRONO_LIBRARY_BASENAME}.dll DESTINATION 3rd)
  INSTALL( FILES ${Boost_LIBRARY_DIR_RELEASE}/${BOOST_REGEX_LIBRARY_BASENAME}.dll DESTINATION 3rd)
ENDIF( WIN32 )

IF( VISR_SYSTEM_NAME MATCHES "Linux" )
  SET( CPACK_GENERATOR DEB TBZ2 )
  SET( CPACK_DEBIAN_HOMEPAGE "http://www.s3a-spatialaudio.org" )
  SET( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
ENDIF(VISR_SYSTEM_NAME MATCHES "Linux")

IF( VISR_SYSTEM_NAME MATCHES "MacOS" )
#  SET( CPACK_GENERATOR productbuild DragNDrop)
   SET( CPACK_GENERATOR productbuild )

##########################################
SET( CPACK_PACKAGE_NAME ${PKG_FILE_NAME}.pkg )
#SET( CPACK_PACKAGE_NAME ${VISR_TOPLEVEL_NAME}-${CMAKE_SYSTEM_NAME}.pkg )
##########################################

  #disabled ZIP and TBZ2 
  #SET( CPACK_GENERATOR Bundle )   
# set( CPACK_PACKAGE_DEFAULT_LOCATION "/Applications" )
# set( CPACK_PACKAGING_INSTALL_PREFIX "${CPACK_PACKAGE_DEFAULT_LOCATION}/${VISR_TOPLEVEL_NAME}")

##########################
	SET( CPACK_PACKAGING_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" )
#	SET( CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_INSTALL_PREFIX} )

###########################  

#SET( CPACK_DMG_BACKGROUND_IMAGE ${CMAKE_SOURCE_DIR}/cmake_modules/resources/s3a_logo.jpg )
#set( CPACK_DMG_VOLUME_NAME ${PKG_FILE_NAME})

######################################################################################################

#  SET( CPACK_BUNDLE_NAME "VISR-0.9.0-Darwin/VISR" )
#  SET( CPACK_BUNDLE_NAME "VISR" )
#  SET( CPACK_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/cmake_modules/resources/s3a_logo.png )
#  SET( CPACK_BUNDLE_PLIST ${CMAKE_SOURCE_DIR}/cmake_modules/Info.plist )
#  SET( CPACK_BUNDLE_STARTUP_COMMAND ${CMAKE_SOURCE_DIR}/cmake_modules/postscript.sh )  

  INSTALL( FILES ${PORTAUDIO_LIBRARIES} DESTINATION 3rd COMPONENT thirdparty_libraries)
  INSTALL( FILES ${SNDFILE_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)  
  INSTALL( FILES ${FLAC_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)
  INSTALL( FILES ${OGG_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)
  INSTALL( FILES ${VORBIS_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)
  INSTALL( FILES ${VORBISENC_LIBRARY} DESTINATION 3rd COMPONENT thirdparty_libraries)

#CPACK_ADD_COMPONENT ( libraries
#  DISPLAY_NAME "Libraries"
#  DESCRIPTION "Static libraries used to build programs with MyLib"
#  INSTALL_TYPES Developer Full )


#set(CPACK_POSTFLIGHT_LOUDSPEAKERLAYOUTS_SCRIPT ${CMAKE_SOURCE_DIR}/cmake_modules/postscript.sh)
configure_file (${CMAKE_SOURCE_DIR}/cmake_modules/postscript.sh.in
        "${PROJECT_BINARY_DIR}/postscript.sh" @ONLY)
set(CPACK_POSTFLIGHT_SCRIPT ${PROJECT_BINARY_DIR}/postscript.sh)

#set(CPACK_POSTUPGRADE_SCRIPT "${CMAKE_SOURCE_DIR}/cmake_modules/pkg_rename.sh" ${PROJECT_BINARY_DIR}/${VISR_TOPLEVEL_NAME}.dmg ${PROJECT_BINARY_DIR}/PKG_FILE_NAME.dmg )

   # CPACK_PACKAGE_FILE_NAME - provides the name of the final compressed disk image (the name of the file that is distributed).
   # CPACK_PACKAGE_ICON - provides the icon for the mounted disk image (appears after the user mounts the disk image).
   

  If( NOT Boost_USE_STATIC_LIBS )
    INSTALL( FILES ${BOOST_S} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${BOOST_T} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${BOOST_FS} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${BOOST_PO} DESTINATION 3rd COMPONENT thirdparty_libraries)

    INSTALL( FILES ${BOOST_C} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${BOOST_DT} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${CMAKE_THREAD_LIBS_INIT} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${BOOST_TI} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${BOOST_A} DESTINATION 3rd COMPONENT thirdparty_libraries)

    INSTALL( FILES ${BOOST_R} DESTINATION 3rd COMPONENT thirdparty_libraries)
    INSTALL( FILES ${CMAKE_THREAD_LIBS_INIT} DESTINATION 3rd COMPONENT thirdparty_libraries)
  ENDIF( NOT Boost_USE_STATIC_LIBS )

ENDIF( VISR_SYSTEM_NAME MATCHES "MacOS" )

INSTALL( DIRECTORY config DESTINATION ${VISR_TOPLEVEL_INSTALL_DIRECTORY} COMPONENT loudspeaker_configs )
INSTALL( FILES ${CMAKE_SOURCE_DIR}/LICENSE.txt DESTINATION ${VISR_TOPLEVEL_INSTALL_DIRECTORY} COMPONENT base )
SET( CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt )

# Install Python example scripts and templates
INSTALL( DIRECTORY src/python/templates DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY} COMPONENT python_templates )
INSTALL( DIRECTORY src/python/packages/visr_bst DESTINATION ${PYTHON_MODULE_INSTALL_DIRECTORY} COMPONENT python_package_bst )

include( CPack )

# Define components and component groups for the component-enabled installers.
# This must happen after include( CPack )

# Installation types are ATM only supported by NSIS.
cpack_add_install_type( developer DISPLAY_NAME Developer )
cpack_add_install_type( full DISPLAY_NAME Full )
cpack_add_install_type( python DISPLAY_NAME Python developer )

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
		   
cpack_add_component(static_libraries
                    DISPLAY_NAME "Static Libraries"
                    DESCRIPTION "Core VISR libraries (static)"
                    DISABLED
		    DEPENDS development_files
                   )

cpack_add_component( standalone_applications
                    DISPLAY_NAME "Standalone applications"
                    DESCRIPTION "Standalone command-line applications"
                    INSTALL_TYPES full
		    DEPENDS shared_libraries
                   )

cpack_add_component(development_files
                    DISPLAY_NAME "Development files"
                    DESCRIPTION description "Header files and CMake support"
                    INSTALL_TYPES developer full
                   )

cpack_add_component( python_externals
                    DISPLAY_NAME "Python Externals"
                    DESCRIPTION "Python modules to access the VISR functionality from Python"
                    INSTALL_TYPES python full
                   )

cpack_add_component_group( python_packages
                          DISPLAY_NAME "Python packages"
		          DESCRIPTION "VISR extension packages implemented in Python"
			 )

cpack_add_component( python_package_bst
                    DISPLAY_NAME "Binaural Synthesis Toolkit"
                    DESCRIPTION "Toolbox for binaural synthesis"
                    INSTALL_TYPES python full
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
		    INSTALL_TYPE developer full
		    GROUP documentation
                   )

cpack_add_component( api_documentation_pdf
                    DISPLAY_NAME "API documentation (PDF)"
                    DESCRIPTION "Code reference documentation"
		    GROUP documentation
                   )

cpack_add_component( doxygen_documentation_html
                    DISPLAY_NAME "Doxygen code documentation"
                    DESCRIPTION "Code reference documentation in Doxygen format (deprecated)"
		    INSTALL_TYPE full
                    DISABLED
		    GROUP documentation
                   )

cpack_add_component( loudspeaker_configs
                    DISPLAY_NAME "Loudspeaker configuration files"
                    DESCRIPTION "Example configuration files for the loudspeaker renderers."
		    INSTALL_TYPE full
                   )
