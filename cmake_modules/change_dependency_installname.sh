#fix install names in a 2nd level dependency.
#arg1 = dependency library name
#arg2 = dependency library path
#arg2 = dependency library desired name
#arg3 = dependent library name

#install_name_tool -id @rpath/$1.dylib $2
# echo "Correct arguments $1 $2"

loadlibname=$(otool -l $2 | grep -F "$1." | awk -F"name "  '{print $2}' | awk -F\(  '{print $1}')
install_name_tool -change $loadlibname @rpath/$1.dylib $2

#install_name_tool -change libboost_filesystem.dylib ../Resources/3rd/libboost_filesystem.dylib ../Resources/bin/network_audio_streamer
