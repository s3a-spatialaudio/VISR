

# Build the FFTS library.
# First, provide hard-coded settings for the FFTS'configuration options
set( GENERATE_POSITION_INDEPENDENT_CODE ON CACHE BOOL
"Force position-independent code for 3rd-party library ffts." FORCE )
#  According to FFTS' CmakeLists.txt, ENABLE_AVX is for debugging only.
set( ENABLE_AVX OFF CACHE BOOL "Disable AVX code generation for ffts." )
if( VISR_SYSTEM_PROCESSOR STREQUAL 'armv7l' )
  set( ENABLE_NEON ON CACHE BOOL "FFTS: Enable double-precision arithmethic." FORCE )
endif( VISR_SYSTEM_PROCESSOR STREQUAL 'armv7l' )
set( ENABLE_DOUBLE ON CACHE BOOL "FFTS: Enable double-precision arithmethic." FORCE )
if( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
  set( ENABLE_SHARED ON CACHE BOOL "FFTS: Enable building a shared library." FORCE )
else( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
  set( ENABLE_SHARED OFF CACHE BOOL "FFTS: Enable building a shared library." FORCE )
endif( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
set( DISABLE_DYNAMIC_CODE ON CACHE BOOL "FFTS: Disable dynamic code generation." FORCE )
# Make these options less visible
mark_as_advanced( FORCE GENERATE_POSITION_INDEPENDENT_CODE ENABLE_AVX ENABLE_NEON ENABLE_DOUBLE ENABLE_SHARED ENABLE_STATIC ENABLE_VFP DISABLE_DYNAMIC_CODE )
add_subdirectory( ffts ${CMAKE_BINARY_DIR}/3rd/ffts )
set( FFTS_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/ffts/include/ffts.h)
if( ENABLE_STATIC )
  set_property( TARGET ffts_static PROPERTY PUBLIC_HEADER
    "${FFTS_PUBLIC_HEADERS}" )
  # Override global visibility preset to avoid warnings.
  # This is necessary because the global setting for policy CMP0063 is not heeded
  # in add_subdirectory() subprojects.
  set_property( TARGET ffts_static PROPERTY C_VISIBILITY_PRESET )
  set_target_properties( ffts_static PROPERTIES FOLDER 3rd)
  get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    DIRECTORY )
  target_include_directories(ffts_static PUBLIC
      $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}/3rd/ffts/include>
      $<INSTALL_INTERFACE:include/visr/3rd/ffts>
  )
  set_target_properties( ffts_static PROPERTIES
    ARCHIVE_OUTPUT_NAME ffts
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/3rd
  )
  install(TARGETS ffts_static
          EXPORT VISR_exports
          PUBLIC_HEADER DESTINATION include/visr/3rd/ffts COMPONENT   development_files
          ARCHIVE DESTINATION 3rd COMPONENT development_files
          )
endif( ENABLE_STATIC )
if( ENABLE_SHARED )
  set_property( TARGET ffts_static PROPERTY PUBLIC_HEADER
    "${FFTS_PUBLIC_HEADERS}" )
  set_property( TARGET ffts_shared PROPERTY FOLDER 3rd)
  set_target_properties( ffts_shared PROPERTIES
    ARCHIVE_OUTPUT_NAME ffts_shared # Import library on Windows
    LIBRARY_OUTPUT_NAME ffts # Shared library on Unix platforms
    RUNTIME_OUTPUT_NAME ffts # DLL on Windows
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/3rd
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/3rd
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/3rd
  )
  get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    DIRECTORY )
  target_include_directories(ffts_shared PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}/3rd/ffts/include>
    $<INSTALL_INTERFACE:include/visr/3rd/ffts>
  )
  install(TARGETS ffts_shared
          EXPORT VISR_exports
          PUBLIC_HEADER DESTINATION include/visr/3rd/ffts COMPONENT   development_files
          ARCHIVE DESTINATION 3rd COMPONENT development_files
          LIBRARY DESTINATION 3rd COMPONENT shared_libraries
          RUNTIME DESTINATION 3rd COMPONENT shared_libraries
          )
endif( ENABLE_SHARED )
# Override global visibility preset to avoid warnings.
# This is necessary because the global setting for policy CMP0063 is not heeded
# in add_subdirectory() subprojects.
set_property( TARGET ffts_cpu_test PROPERTY C_VISIBILITY_PRESET)
set_property( TARGET ffts_test PROPERTY C_VISIBILITY_PRESET )
set_property( TARGET ffts_trig_test PROPERTY C_VISIBILITY_PRESET )
set_property( TARGET ffts_cpu_test PROPERTY FOLDER 3rd/test)
set_property( TARGET ffts_test PROPERTY FOLDER 3rd/test)
set_property( TARGET ffts_trig_test PROPERTY FOLDER 3rd/test)

# KissFFT
if( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
  set( KISSFFT_BUILD_SHARED_LIBRARY OFF CACHE BOOL "KissFFT: Build shared library" FORCE )
else( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
  set( KISSFFT_BUILD_SHARED_LIBRARY OFF CACHE BOOL "KissFFT: Build shared library" FORCE )
endif( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
add_subdirectory( kissfft ${CMAKE_BINARY_DIR}/3rd/kissfft )
set_target_properties( kissfft_static PROPERTIES FOLDER 3rd)
# Override global visibility preset to avoid warnings.
# This is necessary because the global setting for policy CMP0063 is not heeded
# in add_subdirectory() subprojects.
set_property( TARGET kissfft_static PROPERTY C_VISIBILITY_PRESET )
# Overwrite the include_directories
get_filename_component(PARENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  DIRECTORY )
target_include_directories(kissfft_static PUBLIC
    $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}/3rd/kissfft>
    $<INSTALL_INTERFACE:include/visr/3rd/kissfft>
)
install(TARGETS kissfft_static
        EXPORT VISR_exports
        PUBLIC_HEADER DESTINATION include/visr/3rd/kissfft COMPONENT   development_files
        ARCHIVE DESTINATION 3rd COMPONENT development_files
        )

if( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
  set_target_properties( kissfft_shared PROPERTIES FOLDER 3rd)
  target_include_directories(kissfft_shared PUBLIC
      $<BUILD_INTERFACE:${PARENT_SOURCE_DIR}/3rd/kissfft>
      $<INSTALL_INTERFACE:include/visr/3rd/kissfft>
  )
  install(TARGETS kissfft_shared
          EXPORT VISR_exports
          PUBLIC_HEADER DESTINATION include/visr/3rd/kissfft COMPONENT   development_files
          ARCHIVE DESTINATION 3rd COMPONENT development_files
          LIBRARY DESTINATION 3rd COMPONENT shared_libraries
          RUNTIME DESTINATION 3rd COMPONENT shared_libraries
          )
endif( "shared" IN_LIST VISR_BUILD_LIBRARY_TYPES )
