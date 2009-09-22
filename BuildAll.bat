rem @echo off
rem   Build script, uses vcbuild to completetly build FreeCAD

rem  set the aprobiated Variables here or outside in the system
if NOT DEFINED VCDIR set VCDIR="C:\Program Files (x86)\Microsoft Visual Studio 9.0\"

rem Register VS Build programms
call %VCDIR%\VC\vcvarsall.bat"

rem Set Standard include paths
rem set INCLUDE=%INCLUDE%;%FrameworkSDKDir%\include
rem set INCLUDE=%INCLUDE%;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include

rem Set lib Pathes
rem set LIB=%LIB%;C:\Program Files\Microsoft SDKs\Windows\v6.0A\Lib
rem set LIB=%LIB%;%PROGRAMFILES%\Microsoft Visual Studio\VC98\Lib

rem Start the Visuall Studio build process
"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcpackages\vcbuild.exe" FreeCAD_trunk.sln /useenv 
"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcpackages\vcbuild.exe" FreeCAD_trunk.sln /useenv 
