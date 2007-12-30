rem @echo off
rem   Build script, uses vcbuild to completetly build FreeCAD

rem  set the aprobiated Variables here or outside in the system
if NOT DEFINED QMAKESPEC set QMAKESPEC=win32-msvc2005
if NOT DEFINED QTDIR set QTDIR=C:\# Libs\FreeCADLibs\FreeCADLibs5
if NOT DEFINED FREECADLIB set FREECADLIB=C:\# Libs\FreeCADLibs\FreeCADLibs5

set PATH=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem
set INCLUDE=
set LIB=

rem Register VS Build programms
call "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat"
rem QT stuff
set PATH=%QTDIR%\bin;%FREECADLIB%\bin;%FREECADLIB%\dll;%PATH%

rem Set Standard include paths
set INCLUDE=%INCLUDE%;%FrameworkSDKDir%\include
set INCLUDE=%INCLUDE%;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include
set INCLUDE=%INCLUDE%;%FREECADLIB%\include
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\boost-1_33_1
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\coin245
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\gts070_glib
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\OpenCascade62
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\python25
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\soqt141
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\Wm3_VC8
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\xerces260
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\zlib123
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\Qt
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\Qt3Support
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtAssistent
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtCore
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtDeigner
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtGui
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtNetwork
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtOpenGL
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtSql
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtSvg
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtTest
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtUITools
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt414\QtXml
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\OpenCV10

rem Set lib Pathes
set LIB=%LIB%;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib
set LIB=%LIB%;%PROGRAMFILES%\Microsoft Visual Studio\VC98\Lib
set LIB=%LIB%;%QTDIR%\lib
set LIB=%LIB%;%FREECADLIB%\lib

rem Start the Visuall Studio build process
"C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcpackages\vcbuild.exe" FreeCAD.sln /useenv 



rem call "C:\Programme\Microsoft Visual Studio 8\VC\vcvarsall.bat"
rem MSBuild.exe FreeCAD.sln /p:Configuration=RELEASE
