# Microsoft Developer Studio Project File - Name="AppRaytracing" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AppRaytracing - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "AppRaytracing.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppRaytracing.mak" CFG="AppRaytracing - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppRaytracing - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppRaytracing - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppRaytracing - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPRaytracing_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../" /D "NDEBUG" /D "_USRDLL" /D "FCAppRaytracing" /Yu"PreCompiled.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 MSVCRT.LIB MSVCPRT.LIB python23.lib kernel32.lib TKernel.lib TKMath.lib TKService.lib TKGeomAlgo.lib TKGeomBase.lib TKG2d.lib TKG3d.lib TKBRep.lib TKTopAlgo.lib TKPrim.lib TKXSBase.lib TKMesh.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release/Raytracing.pyd"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy   /y   /f   Release\Raytracing.pyd   ..\..\..\..\Mod\Raytracing\  	xcopy   /y   /f   ..\Init.py   ..\..\..\..\Mod\Raytracing\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AppRaytracing - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPRaytracing_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../" /D "_USRDLL" /D "FCAppRaytracing" /D "FC_DEBUG" /FR /Yu"PreCompiled.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRTD.LIB MSVCPRTD.LIB python23_d.lib kernel32.lib TKernel.lib TKMath.lib TKService.lib TKGeomAlgo.lib TKGeomBase.lib TKG2d.lib TKG3d.lib TKBRep.lib TKTopAlgo.lib TKPrim.lib TKXSBase.lib TKMesh.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug\Raytracing_d.pyd" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy   /y   /f   Debug\Raytracing_d.pyd   ..\..\..\..\Mod\Raytracing\  	xcopy   /y   /f   ..\Init.py   ..\..\..\..\Mod\Raytracing\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AppRaytracing - Win32 Release"
# Name "AppRaytracing - Win32 Debug"
# Begin Source File

SOURCE=.\AppRaytracing.cpp
# End Source File
# Begin Source File

SOURCE=.\AppRaytracingPy.cpp
# End Source File
# Begin Source File

SOURCE=.\FreeCADpov

!IF  "$(CFG)" == "AppRaytracing - Win32 Release"

# Begin Custom Build - Building FreeCADpov.h
InputPath=.\FreeCADpov

"FreeCADpov.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist "%FREECADLIB%" (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\..\..\Tools\PythonToCPP.py FreeCADpov FreeCADpov.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AppRaytracing - Win32 Debug"

# Begin Custom Build - Building FreeCADpov.h
InputPath=.\FreeCADpov

"FreeCADpov.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist "%FREECADLIB%" (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\..\..\Tools\PythonToCPP.py FreeCADpov FreeCADpov.h 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PovTools.cpp
# End Source File
# Begin Source File

SOURCE=.\PovTools.h
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\RayFeature.cpp
# End Source File
# Begin Source File

SOURCE=.\RayFeature.h
# End Source File
# End Target
# End Project
