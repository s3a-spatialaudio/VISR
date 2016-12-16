# Copyright Institute of Sound and Vibration Research - All rights reserved

INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR})

SET( SOURCES mex_wrapper.cpp )

# Basically, this makes the files show up in the Visual studio project.
SET( HEADERS export_symbol.hpp mex_wrapper.hpp )

ADD_LIBRARY( mexsupport STATIC ${SOURCES} ${HEADERS} )
