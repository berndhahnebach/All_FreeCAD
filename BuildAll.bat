rem @echo off
rem   Build script, uses vcbuild to completetly build FreeCAD

rem  set the aprobiated Variables here or outside in the system
if NOT DEFINED VCDIR set VCDIR=C:\Program Files\Microsoft Visual Studio 9.0

rem Register VS Build programms
call "%VCDIR%\VC\vcvarsall.bat"


rem Start the Visuall Studio build process
"%VCDIR%\VC\vcpackages\vcbuild.exe" FreeCAD_trunk.sln "Debug|Win32" 
"%VCDIR%\VC\vcpackages\vcbuild.exe" FreeCAD_trunk.sln "Debug|Win32" 
"%VCDIR%\VC\vcpackages\vcbuild.exe" FreeCAD_trunk.sln "Release|Win32" 
