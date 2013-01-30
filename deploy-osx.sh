#! /bin/bash

###                                                    ###
#    This script is used to create a bundle for OS X     #
###                                                    ###


# Set up the basic app
mkdir -p Traverso.app/Contents/MacOS/
mkdir -p Traverso.app/Contents/Resources/
mkdir -p Traverso.app/Contents/Frameworks/

cp bin/Release/traverso Traverso.app/Contents/MacOS/
cp resources/images/traverso_mac.icns Traverso.app/Contents/Resources/Traverso.icns
cp resources/Info.plist Traverso.app/Contents/
cp /opt/local/bin/cdrdao Traverso.app/Contents/MacOS/
cp /opt/local/bin/sox Traverso.app/Contents/MacOS/

# copy in sox-specific dependencies
cp /opt/local/lib/libsox.2.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libpng15.15.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libmagic.1.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libopencore-amrnb.0.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libopencore-amrwb.0.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libid3tag.0.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libz.1.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libtwolame.0.dylib Traverso.app/Contents/Frameworks
cp /opt/local/lib/libiconv.2.dylib Traverso.app/Contents/Frameworks

# let QT do most of the deployment work
macdeployqt Traverso.app

# fix a macdeployqt bug
cp /opt/local/lib/db46/libdb-4.6.dylib Traverso.app/Contents/Frameworks
install_name_tool -id @executable_path/../Frameworks/libdb-4.6.dylib Traverso.app/Contents/Frameworks/libdb-4.6.dylib
install_name_tool -change /opt/local/lib/db46/libdb-4.6.dylib @executable_path/../Frameworks/libdb-4.6.dylib Traverso.app/Contents/Frameworks/librdf.0.dylib

# update sox dependencies
install_name_tool -id @executable_path/../Frameworks/libsox.2.dylib Traverso.app/Contents/Frameworks/libsox.2.dylib
install_name_tool -id @executable_path/../Frameworks/libpng15.15.dylib Traverso.app/Contents/Frameworks/libpng15.15.dylib
install_name_tool -id @executable_path/../Frameworks/libmagic.1.dylib Traverso.app/Contents/Frameworks/libmagic.1.dylib
install_name_tool -id @executable_path/../Frameworks/libopencore-amrnb.0.dylib Traverso.app/Contents/Frameworks/libopencore-amrnb.0.dylib
install_name_tool -id @executable_path/../Frameworks/libopencore-amrwb.0.dylib Traverso.app/Contents/Frameworks/libopencore-amrwb.0.dylib
install_name_tool -id @executable_path/../Frameworks/libid3tag.0.dylib Traverso.app/Contents/Frameworks/libid3tag.0.dylib
install_name_tool -id @executable_path/../Frameworks/libz.1.dylib Traverso.app/Contents/Frameworks/libz.1.dylib
install_name_tool -id @executable_path/../Frameworks/libtwolame.0.dylib Traverso.app/Contents/Frameworks/libtwolame.0.dylib
install_name_tool -id @executable_path/../Frameworks/libiconv.2.dylib Traverso.app/Contents/Frameworks/libiconv.2.dylib

install_name_tool -change /opt/local/lib/libvorbis.0.dylib @executable_path/../Frameworks/libvorbis.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libvorbisfile.3.dylib @executable_path/../Frameworks/libvorbisfile.3.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libvorbisenc.2.dylib @executable_path/../Frameworks/libvorbisenc.2.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libogg.0.dylib @executable_path/../Frameworks/libogg.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libmad.0.dylib @executable_path/../Frameworks/libmad.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libwavpack.1.dylib @executable_path/../Frameworks/libwavpack.1.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libFLAC.8.dylib @executable_path/../Frameworks/libFLAC.8.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libmp3lame.0.dylib @executable_path/../Frameworks/libmp3lame.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libsamplerate.0.dylib @executable_path/../Frameworks/libsamplerate.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libsox.2.dylib @executable_path/../Frameworks/libsox.2.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libpng15.15.dylib @executable_path/../Frameworks/libpng15.15.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libmagic.1.dylib @executable_path/../Frameworks/libmagic.1.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libopencore-amrnb.0.dylib @executable_path/../Frameworks/libopencore-amrnb.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libopencore-amrwb.0.dylib @executable_path/../Frameworks/libopencore-amrwb.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libid3tag.0.dylib @executable_path/../Frameworks/libid3tag.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libtwolame.0.dylib @executable_path/../Frameworks/libtwolame.0.dylib Traverso.app/Contents/MacOS/sox
install_name_tool -change /opt/local/lib/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib Traverso.app/Contents/MacOS/sox

install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib Traverso.app/Contents/Frameworks/libid3tag.0.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib Traverso.app/Contents/Frameworks/libmagic.1.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib Traverso.app/Contents/Frameworks/libpng15.15.dylib
