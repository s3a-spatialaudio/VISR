# Copyright Institute of Sound and Vibration Research - All rights reserved

add_subdirectory( visr )

add_subdirectory( efl )

add_subdirectory( signalflows )

add_subdirectory( rbbl )

add_subdirectory( rcl )

add_subdirectory( rrl )

add_subdirectory( panning )

add_subdirectory( pml )

add_subdirectory( objectmodel )

add_subdirectory( reverbobject )

add_subdirectory( audiointerfaces )

add_subdirectory( libpythonbindinghelpers )

# "Meta-target" combining all Python externals to serve as a single dependency.
add_custom_target( pythonexternals
  DEPENDS visrpython eflpython signalflowspython rbblpython
  rclpython rrlpython panningpython pmlpython
  objectmodelpython objectmodelpython reverbobjectpython
  audiointerfacespython )
