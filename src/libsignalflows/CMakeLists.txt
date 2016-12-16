# Copyright Institute of Sound and Vibration Research - All rights reserved

<<<<<<< HEAD
SET( SOURCES baseline_renderer.cpp delay_vector.cpp gain_matrix.cpp matrix_convolver.cpp)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS baseline_renderer.hpp delay_vector.hpp gain_matrix.hpp matrix_convolver.hpp )
=======
SET( SOURCES 
core_renderer.cpp
baseline_renderer.cpp 
bunch_renderer.cpp 
delay_vector.cpp 
gain_matrix.cpp
time_frequency_feedthrough.cpp
)

# Basically, this makes the headers show up in the Visual studio project.
SET( HEADERS 
baseline_renderer.hpp
bunch_renderer.hpp
core_renderer.hpp
delay_vector.hpp 
gain_matrix.hpp
time_frequency_feedthrough.hpp
)
>>>>>>> 3b8f1889c6e66d81bd3c390bfd7f720dbfdacaa7

ADD_LIBRARY( signalflows STATIC ${SOURCES} ${HEADERS} )

# Add the unit test directory.
ADD_SUBDIRECTORY( test )
