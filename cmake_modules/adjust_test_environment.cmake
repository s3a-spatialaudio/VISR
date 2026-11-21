# Copyright (c) 2014-2020 Andreas Franck a.franck@soton.ac.uk --- All rights reserved.
# Copyright (c) 2014-2019 Institute of Sound and Vibration Research,
#   University of Southampton and VISR contributors --- All rights reserved.

# Adjust the system environment of a unit test target.
# This is mainly needed on Windows to put the paths containing the 3rd party
# libraries and the VISR DLLs on the system path.
# Additional arguments are directories prepended to the system path (after the
# VISR build directories), e.g., the directory containing the Python DLL.
function( adjustTestEnvironment target )
  if(VISR_SYSTEM_NAME MATCHES "Windows")
    # Prevent CMake from interpreting the semicolon (the separator in Windows path lists) as list separator.
    # Double quoting required because the variable is evaluated twice.
    string(REPLACE ";" "\\\;"  QUOTED_PATH "$ENV{PATH}" )
    set( _extraPath "" )
    foreach( _dir IN LISTS ARGN )
      string( APPEND _extraPath "${_dir}\;" )
    endforeach()
    if( BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "shared" )
      set_property(TEST ${target} APPEND PROPERTY ENVIRONMENT
        "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;$<TARGET_FILE_DIR:visr_shared>\;${_extraPath}${QUOTED_PATH}" )
    else()
     set_property(TEST ${target} APPEND PROPERTY ENVIRONMENT
        "PATH=${VISR_BUILD_3RD_PARTY_RUNTIME_LIBRARY_DIR}\;${_extraPath}${QUOTED_PATH}" )
    endif( BUILD_LIBRARY_TYPE_FOR_APPS STREQUAL "shared" )
  endif(VISR_SYSTEM_NAME MATCHES "Windows")
endfunction()


# Determine the Python home (sys.base_prefix) and, if Python_EXECUTABLE belongs
# to a virtual environment, the environment's site-packages directory.
# Results are cached and only recomputed when Python_EXECUTABLE changes.
#
# Rationale: An embedded Python interpreter (e.g., in C++ unit tests) on Windows
# locates its standard library relative to the host executable or via the
# registry key Software\Python\PythonCore\<ver>\PythonPath. Neither works if the
# test binary lives in the build tree and the Python installation did not write
# that key (e.g., installations made by the Python install manager), resulting in
# "ModuleNotFoundError: No module named 'encodings'". Setting PYTHONHOME avoids that.
function( visrDeterminePythonTestPaths )
  if( NOT Python_EXECUTABLE )
    return()
  endif( NOT Python_EXECUTABLE )
  if( DEFINED CACHE{VISR_PYTHON_TEST_PATHS_EXECUTABLE}
      AND VISR_PYTHON_TEST_PATHS_EXECUTABLE STREQUAL Python_EXECUTABLE )
    return() # Cached values are up to date.
  endif()
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -c
      "import sys, sysconfig; f = lambda p: p.replace('\\\\', '/'); print(f(sys.base_prefix)); print(f(sysconfig.get_paths()['purelib']) if sys.prefix != sys.base_prefix else '')"
    OUTPUT_VARIABLE _pyOutput
    RESULT_VARIABLE _pyResult
    OUTPUT_STRIP_TRAILING_WHITESPACE )
  if( NOT _pyResult EQUAL 0 )
    message( WARNING "Could not query Python paths from ${Python_EXECUTABLE}; PYTHONHOME will not be set for unit tests." )
    set( _pyHome "" )
    set( _pyVenvSitePackages "" )
  else()
    string( REPLACE "\n" ";" _pyLines "${_pyOutput}" )
    list( GET _pyLines 0 _pyHome )
    list( LENGTH _pyLines _pyNumLines )
    if( _pyNumLines GREATER 1 )
      list( GET _pyLines 1 _pyVenvSitePackages )
    else()
      set( _pyVenvSitePackages "" )
    endif( _pyNumLines GREATER 1 )
  endif( NOT _pyResult EQUAL 0 )
  set( VISR_PYTHON_TEST_HOME "${_pyHome}" CACHE INTERNAL "Python home used for unit tests." )
  set( VISR_PYTHON_TEST_VENV_SITE_PACKAGES "${_pyVenvSitePackages}" CACHE INTERNAL
    "site-packages of the virtual environment used for unit tests (empty if no venv)." )
  set( VISR_PYTHON_TEST_PATHS_EXECUTABLE "${Python_EXECUTABLE}" CACHE INTERNAL
    "Python executable the cached Python test paths were determined for." )
endfunction()

# Put directories in front of the PYTHONPATH of a unit test target.
# The directories (passed as additional arguments) end up in the given order.
function( visrPrependTestPythonPath target )
  if( CMAKE_VERSION VERSION_LESS "3.22.0" )
    # ideally we would add to PYTHONPATH, but the path separator is
    # system-dependant, and the sensible way to do it would be to use
    # cmake_path or cmake -E env, but those don't help expand the support base
    # (much), so just ignore it with a warning
    message(WARNING "packages on PYTHONPATH will be ignored in tests; upgrade to cmake 3.22")
    if( WIN32 )
      list( JOIN ARGN "\;" _pythonPath )
    else()
      list( JOIN ARGN ":" _pythonPath )
    endif( WIN32 )
    set_property( TEST ${target} APPEND PROPERTY ENVIRONMENT
      "PYTHONPATH=${_pythonPath}" )
  else()
    # Each path_list_prepend puts its directory in front, so process them in reverse.
    set( _dirs ${ARGN} )
    list( REVERSE _dirs )
    foreach( _dir IN LISTS _dirs )
      set_property( TEST ${target} APPEND PROPERTY ENVIRONMENT_MODIFICATION
        "PYTHONPATH=path_list_prepend:${_dir}" )
    endforeach()
  endif( CMAKE_VERSION VERSION_LESS "3.22.0" )
endfunction()

# Adjust the system environment of a Python-based unit test target.
# In addition to the adjustments needed for all unit tests, the PYTHONPATH must contain
# the directory holding all used VISR Python externals.
# This is done by passing a CMake target pythonpackage.
function( adjustPythonTestEnvironment target pythonpackage )
  adjustTestEnvironment( ${target} )
  visrPrependTestPythonPath( ${target} $<TARGET_FILE_DIR:${pythonpackage}> )
endfunction()

# Adjust the system environment of a unit test target whose executable embeds a
# Python interpreter (as opposed to tests run by the Python interpreter itself,
# see adjustPythonTestEnvironment()).
# In addition to the adjustments of adjustPythonTestEnvironment(), on Windows:
# * PYTHONHOME is set to the base installation (sys.base_prefix), so that the
#   embedded interpreter finds the standard library (see visrDeterminePythonTestPaths()).
# * If Python_EXECUTABLE belongs to a virtual environment, the environment's
#   site-packages directory is added to the PYTHONPATH, because an embedded
#   interpreter is not aware of the venv (there is no pyvenv.cfg next to the
#   test executable).
# * The base installation is put on the system path because it holds the
#   Python DLL (python3XX.dll), which a venv does not contain. Without it, the
#   test executable fails to start (STATUS_DLL_NOT_FOUND, 0xc0000135).
# On all platforms, the memory leak detection of Boost.Test is disabled (it is
# only active in MSVC debug builds anyway): the extension modules imported by
# the embedded interpreter leave a fixed set of allocations (e.g., pybind11
# type data), which are not freed at interpreter finalisation because CPython
# never unloads extension modules. These reports do not grow with the number
# of test cases and would hide genuine leaks.
function( adjustEmbeddedPythonTestEnvironment target pythonpackage )
  set( _venvSitePackages "" )
  set( _pythonDllDir "" )
  if( VISR_SYSTEM_NAME MATCHES "Windows" )
    visrDeterminePythonTestPaths()
    if( VISR_PYTHON_TEST_HOME )
      set_property( TEST ${target} APPEND PROPERTY ENVIRONMENT
        "PYTHONHOME=${VISR_PYTHON_TEST_HOME}" )
      set( _venvSitePackages "${VISR_PYTHON_TEST_VENV_SITE_PACKAGES}" )
      set( _pythonDllDir "${VISR_PYTHON_TEST_HOME}" )
    endif( VISR_PYTHON_TEST_HOME )
  endif( VISR_SYSTEM_NAME MATCHES "Windows" )
  adjustTestEnvironment( ${target} ${_pythonDllDir} )
  # The VISR package directory comes first, the venv's site-packages (if any) after it.
  visrPrependTestPythonPath( ${target} $<TARGET_FILE_DIR:${pythonpackage}> ${_venvSitePackages} )
  set_property( TEST ${target} APPEND PROPERTY ENVIRONMENT
    "BOOST_TEST_DETECT_MEMORY_LEAK=0" )
endfunction()
