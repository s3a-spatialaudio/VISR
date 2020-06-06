# Copyright Institute of Sound and Vibration Research - All rights reserved

INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

SET( SOURCES mex_wrapper.cpp )

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS export_symbol.hpp mex_wrapper.hpp )

add_library( mexsupport STATIC ${SOURCES} ${HEADERS} )
# Transitive link dependencies
# This also helps us to avoid writing the Matlab library dependencies for all our externals.
target_link_libraries( mexsupport ${MATLAB_MEX_LIBRARY} )
target_link_libraries( mexsupport ${MATLAB_MX_LIBRARY} )
