# Microsoft Developer Studio Project File - Name="FreeCADBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FreeCADBase - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADBase.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADBase.mak" CFG="FreeCADBase - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FreeCADBase - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FreeCADBase - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeCADBase - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FREECADBASE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /D "_USRDLL" /D "FCBase" /Yu"PreCompiled.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 MSVCRT.LIB MSVCPRT.LIB kernel32.lib xerces-c_2.lib zlib1.lib python23.lib /nologo /dll /machine:I386 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy   /y   /f   Release\FreeCADBase.dll   ..\..\bin\  	xcopy   /y   /f   Release\FreeCADBase.lib   ..\..\lib\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FreeCADBase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FREECADBASE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /D "_USRDLL" /D "FCBase" /D "FC_DEBUG" /Yu"PreCompiled.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRTD.LIB MSVCPRTD.LIB kernel32.lib xerces-c_2D.lib zlib1.lib python23_d.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug/FreeCADBaseD.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy   /Y   /f    Debug\FreeCADBaseD.dll   ..\..\bin\  	xcopy   /Y   /f   Debug\FreeCADBaseD.lib   ..\..\lib\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FreeCADBase - Win32 Release"
# Name "FreeCADBase - Win32 Debug"
# Begin Group "zipios"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\zipios\backbuffer.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\basicentry.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\basicentry.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\collcoll.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\collcoll.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\deflateoutputstreambuf.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\deflateoutputstreambuf.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\dircoll.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\dircoll.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\directory.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\directory.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\fcoll.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\fcoll.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\fcollexceptions.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\fcollexceptions.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\fileentry.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\fileentry.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\filepath.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\filepath.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\filterinputstreambuf.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\filterinputstreambuf.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\filteroutputstreambuf.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\filteroutputstreambuf.h"
# End Source File
# Begin Source File

SOURCE=.\zipios\gzipoutputstream.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zipios\gzipoutputstream.h
# End Source File
# Begin Source File

SOURCE=.\zipios\gzipoutputstreambuf.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\zipios\gzipoutputstreambuf.h
# End Source File
# Begin Source File

SOURCE=".\zipios\inflateinputstreambuf.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\inflateinputstreambuf.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\meta-iostreams.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\outputstringstream.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\simplesmartptr.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\virtualseeker.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipfile.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\zipfile.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\ziphead.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\ziphead.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipheadio.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\zipheadio.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipinputstream.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\zipinputstream.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipinputstreambuf.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\zipinputstreambuf.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipios-config.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipios_common.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipios_defs.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipoutputstream.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\zipoutputstream.h"
# End Source File
# Begin Source File

SOURCE=".\zipios\zipoutputstreambuf.cpp"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=".\zipios\zipoutputstreambuf.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\Base.cpp
# End Source File
# Begin Source File

SOURCE=.\Base.h
# End Source File
# Begin Source File

SOURCE=.\BoundBox.cpp

!IF  "$(CFG)" == "FreeCADBase - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADBase - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BoundBox.h
# End Source File
# Begin Source File

SOURCE=.\Builder3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Builder3D.h
# End Source File
# Begin Source File

SOURCE=.\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\Console.h
# End Source File
# Begin Source File

SOURCE=.\DllMain.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Doxygen.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\EnvMacros.h
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# End Source File
# Begin Source File

SOURCE=.\Exception.h
# End Source File
# Begin Source File

SOURCE=.\Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Factory.h
# End Source File
# Begin Source File

SOURCE=.\FileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\FileInfo.h
# End Source File
# Begin Source File

SOURCE=.\FileTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\FileTemplate.h
# End Source File
# Begin Source File

SOURCE=.\gzstream.cpp
# End Source File
# Begin Source File

SOURCE=.\gzstream.h
# End Source File
# Begin Source File

SOURCE=.\Handle.cpp
# End Source File
# Begin Source File

SOURCE=.\Handle.h
# End Source File
# Begin Source File

SOURCE=.\InputSource.cpp
# End Source File
# Begin Source File

SOURCE=.\InputSource.h
# End Source File
# Begin Source File

SOURCE=.\Interpreter.cpp
# End Source File
# Begin Source File

SOURCE=.\Interpreter.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp

!IF  "$(CFG)" == "FreeCADBase - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADBase - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Observer.h
# End Source File
# Begin Source File

SOURCE=.\Parameter.cpp
# End Source File
# Begin Source File

SOURCE=.\Parameter.h
# End Source File
# Begin Source File

SOURCE=.\ParameterPy.cpp
# End Source File
# Begin Source File

SOURCE=.\Persistance.cpp
# End Source File
# Begin Source File

SOURCE=.\Persistance.h
# End Source File
# Begin Source File

SOURCE=.\Placement.cpp
# End Source File
# Begin Source File

SOURCE=.\Placement.h
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\PyExport.cpp
# End Source File
# Begin Source File

SOURCE=.\PyExport.h
# End Source File
# Begin Source File

SOURCE=.\PyExportImp.cpp
# End Source File
# Begin Source File

SOURCE=.\PyExportImp.h
# End Source File
# Begin Source File

SOURCE=.\PyTools.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\PyTools.h
# End Source File
# Begin Source File

SOURCE=.\Reader.cpp
# End Source File
# Begin Source File

SOURCE=.\Reader.h
# End Source File
# Begin Source File

SOURCE=.\Sequencer.cpp
# End Source File
# Begin Source File

SOURCE=.\Sequencer.h
# End Source File
# Begin Source File

SOURCE=.\Stream.cpp
# End Source File
# Begin Source File

SOURCE=.\Stream.h
# End Source File
# Begin Source File

SOURCE=.\Swap.cpp
# End Source File
# Begin Source File

SOURCE=.\Swap.h
# End Source File
# Begin Source File

SOURCE=.\TimeInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeInfo.h
# End Source File
# Begin Source File

SOURCE=.\Tools2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Tools2D.h
# End Source File
# Begin Source File

SOURCE=.\Type.cpp
# End Source File
# Begin Source File

SOURCE=.\Type.h
# End Source File
# Begin Source File

SOURCE=.\Vector3D.cpp

!IF  "$(CFG)" == "FreeCADBase - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADBase - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Vector3D.h
# End Source File
# Begin Source File

SOURCE=.\ViewProj.h
# End Source File
# Begin Source File

SOURCE=.\Writer.cpp
# End Source File
# Begin Source File

SOURCE=.\Writer.h
# End Source File
# End Target
# End Project
