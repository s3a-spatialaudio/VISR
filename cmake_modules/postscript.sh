#!/bin/sh

#temp testing script
#echo "HELLO"
#sudo mv /Applications/VISR-0.9.0/3rd /Applications/VISR-0.9.0/3rd_lib

#PYTHONROOT="$(python3-config --prefix)"
#PYTHONROOT="$(which python3)"
#PYTHONROOT="$(dirname -- $(dirname -- $(which python3)))"
#PYTHONROOT="$(echo $PYTHONHOME)"
#for filename in /Applications/VISR-0.9.0/bin/*; do
#sudo install_name_tool -add_rpath "${PYTHONROOT}" $filename
#done

#for filename in /Applications/VISR-0.9.0/dawplugins/vst/*.vst/Contents/MacOS/*; do
#sudo install_name_tool -add_rpath "${PYTHONROOT}/lib" $filename
#done

#sudo echo "python root: ${PYTHONROOT}/lib" > "/Applications/VISR-0.9.0/log.txt"

#sudo touch "/Applications/VISR-0.9.0/log.txt"
#sudo chmod 777 "/Applications/VISR-0.9.0/log.txt"
#echo $PYTHONHOME 2> "/Applications/VISR-0.9.0/log.txt"
#echo $PYTHONHOME > "/Applications/VISR-0.9.0/log.txt"
#echo $PWD > "/Applications/VISR-0.9.0/log.txt"
#echo $PATH > "/Applications/VISR-0.9.0/log.txt"

PYCONDA="${HOME}/anaconda3/lib"
PYPURE="/Library/Frameworks/Python.framework/Versions/3.6/lib"

#TODO: use a temp_script.sh.in with placeholder for the PYTHON_LIBRARY which is being used in the machine which generates the installer
#TODO: test it with a machine using anaconda
sudo install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.6/Python @rpath/libpython3.6m.dylib /Applications/VISR-0.9.0/lib/libpythonsupport.dylib
sudo install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.6/Python @rpath/libpython3.6m.dylib /Applications/VISR-0.9.0/lib/libpythoncomponents.dylib
sudo install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.6/Python @rpath/libpython3.6m.dylib /Applications/VISR-0.9.0/lib/libsignalflows.dylib

sudo install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.6/Python @rpath/libpython3.6m.dylib /Applications/VISR-0.9.0/bin/baseline_renderer
sudo install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.6/Python @rpath/libpython3.6m.dylib /Applications/VISR-0.9.0/bin/matrix_convolver
sudo install_name_tool -change /Library/Frameworks/Python.framework/Versions/3.6/Python @rpath/libpython3.6m.dylib /Applications/VISR-0.9.0/bin/visr_renderer


if [ -d "${HOME}/anaconda3" ]; then

sudo install_name_tool -add_rpath "${PYCONDA}" /Applications/VISR-0.9.0/lib/libpythonsupport.dylib
sudo install_name_tool -add_rpath "${PYCONDA}" /Applications/VISR-0.9.0/lib/libpythoncomponents.dylib
sudo install_name_tool -add_rpath "${PYCONDA}" /Applications/VISR-0.9.0/lib/libsignalflows.dylib

sudo install_name_tool -add_rpath "${PYCONDA}" /Applications/VISR-0.9.0/bin/baseline_renderer
sudo install_name_tool -add_rpath "${PYCONDA}" /Applications/VISR-0.9.0/bin/matrix_convolver
sudo install_name_tool -add_rpath "${PYCONDA}" /Applications/VISR-0.9.0/bin/visr_renderer

echo "conda python present"
fi


if [ -d "/Library/Frameworks/Python.framework" ]; then

sudo install_name_tool -add_rpath "${PYPURE}" /Applications/VISR-0.9.0/lib/libpythonsupport.dylib
sudo install_name_tool -add_rpath "${PYPURE}" /Applications/VISR-0.9.0/lib/libpythoncomponents.dylib
sudo install_name_tool -add_rpath "${PYPURE}" /Applications/VISR-0.9.0/lib/libsignalflows.dylib

sudo install_name_tool -add_rpath "${PYPURE}" /Applications/VISR-0.9.0/bin/baseline_renderer
sudo install_name_tool -add_rpath "${PYPURE}" /Applications/VISR-0.9.0/bin/matrix_convolver
sudo install_name_tool -add_rpath "${PYPURE}" /Applications/VISR-0.9.0/bin/visr_renderer

echo "system python present"
fi
