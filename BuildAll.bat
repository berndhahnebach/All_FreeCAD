@echo off
rem   Build script, uses vcbuild to completetly build FreeCAD

rem  set the aprobiated Variables here or outside in the system
if NOT DEFINED QMAKESPEC set QMAKESPEC=win32-msvc2005
if NOT DEFINED QTDIR set QTDIR=D:\Develop\Projekte\FreeCAD\FreeCADLibPack\FreeCADLibs4
if NOT DEFINED FREECADLIB set FREECADLIB=D:\Develop\Projekte\FreeCAD\FreeCADLibPack\FreeCADLibs4

set PATH=C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem
set INCLUDE=
set LIB=

rem Register VS Build programms
call "%PROGRAMFILES%\Microsoft Visual Studio 8\VC\vcvarsall.bat"
rem QT stuff
set PATH=%QTDIR%\bin;%FREECADLIB%\bin;%FREECADLIB%\dll;%PATH%

rem Set Standard include paths
set INCLUDE=%INCLUDE%;%FrameworkSDKDir%\include
set INCLUDE=%INCLUDE%;%PROGRAMFILES%\Microsoft Platform SDK\Include
set INCLUDE=%INCLUDE%;%FREECADLIB%\include
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\boost-1_33_1
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\coin245
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\gts070_glib
set INCLUDE=%INCLUDE%;%FREECADLIB%\include\OpenCascade61
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
set INCLUDE=%INCLUDE%;D:\Develop\Projekte\FreeCAD\FreeCADLibPack\FreeCADLibs5\include\OpenCV10

rem Set lib Pathes
set LIB=%LIB%;%PROGRAMFILES%\Microsoft Platform SDK\Lib
set LIB=%LIB%;%PROGRAMFILES%\Microsoft Visual Studio\VC98\Lib
set LIB=%LIB%;%QTDIR%\lib
set LIB=%LIB%;%FREECADLIB%\lib
set LIB=%LIB%;D:\Develop\Projekte\FreeCAD\FreeCADLibPack\FreeCADLibs5\lib

rem Start the Visuall Studio build process
vcbuild D:\Develop\Projekte\FreeCAD\FreeCAD_0.7\FreeCAD.sln /useenv 



rem call "C:\Programme\Microsoft Visual Studio 8\VC\vcvarsall.bat"
rem MSBuild.exe FreeCAD.sln /p:Configuration=RELEASE
