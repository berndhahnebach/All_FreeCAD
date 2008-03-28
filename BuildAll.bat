rem @echo off
rem   Build script, uses vcbuild to completetly build FreeCAD

rem  set the aprobiated Variables here or outside in the system
if NOT DEFINED QMAKESPEC set QMAKESPEC=win32-msvc2005
if NOT DEFINED QTDIR set QTDIR=C:\Libs\FreeCADLibs\FreeCADLibs6.0
if NOT DEFINED FREECADLIB set FREECADLIB=C:\Libs\FreeCADLibs\FreeCADLibs6.0

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
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\boost
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\coin
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\OpenCascade
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\python
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\soqt
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\xerces
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\zlib
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\Qt
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtAssistent
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtCore
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtDeigner
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtGui
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtNetwork
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtOpenGL
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtSql
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtSvg
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtTest
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtUITools
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\qt\QtXml
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\OpenCV
set INCLUDE=%INCLUDE%;.\include\

rem Set lib Pathes
set LIB=%LIB%;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib
set LIB=%LIB%;%PROGRAMFILES%\Microsoft Visual Studio\VC98\Lib
set LIB=%LIB%;%QTDIR%\lib
set LIB=%LIB%;%FREECADLIB%\lib
set LIB=%LIB%;.\lib

rem Start the Visuall Studio build process
"C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcpackages\vcbuild.exe" FreeCAD.sln /useenv 



rem call "C:\Programme\Microsoft Visual Studio 8\VC\vcvarsall.bat"
rem MSBuild.exe FreeCAD.sln /p:Configuration=RELEASE
