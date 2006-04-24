# Microsoft Developer Studio Project File - Name="AppPart" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AppPart - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AppPart.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AppPart.mak" CFG="AppPart - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AppPart - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppPart - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppPart - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPPART_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../" /D "NDEBUG" /D "_USRDLL" /D "FCAppPart" /Yu"PreCompiled.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 MSVCRT.LIB MSVCPRT.LIB kernel32.lib python23.lib TKernel.lib TKBRep.lib TKMath.lib TKService.lib TKGeomAlgo.lib TKGeomBase.lib TKBool.lib TKBO.lib TKG2d.lib TKG3d.lib TKBRep.lib TKTopAlgo.lib TKPrim.lib TKXSBase.lib TKIGES.lib TKShHealing.lib TKSTEP.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release\Part.pyd"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy   /y   /f   Release\Part.pyd   ..\..\..\..\Mod\Part\  	xcopy   /y   /f   ..\Init.py   ..\..\..\..\Mod\Part\  	xcopy   /y   /f   Release\Part.lib   ..\..\..\..\Mod\Part\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPPART_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../" /D "_USRDLL" /D "FCAppPart" /D "FC_DEBUG" /Fr /Yu"PreCompiled.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRTD.LIB MSVCPRTD.LIB kernel32.lib python23_d.lib TKernel.lib TKBRep.lib TKMath.lib TKService.lib TKGeomAlgo.lib TKGeomBase.lib TKBool.lib TKBO.lib TKG2d.lib TKG3d.lib TKBRep.lib TKTopAlgo.lib TKPrim.lib TKXSBase.lib TKIGES.lib TKShHealing.lib TKSTEP.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug\Part_d.pyd" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy   /y   /f   Debug\Part_d.pyd   ..\..\..\..\Mod\Part\  	xcopy   /y   /f   ..\Init.py   ..\..\..\..\Mod\Part\  	xcopy   /y   /f   Debug\Part_d.lib   ..\..\..\..\Mod\Part\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AppPart - Win32 Release"
# Name "AppPart - Win32 Debug"
# Begin Source File

SOURCE=.\AppPart.cpp
# End Source File
# Begin Source File

SOURCE=.\AppPartPy.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartBox.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartBox.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartCurveNet.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartCurveNet.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartCut.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartCut.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartImportBrep.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartImportBrep.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartImportIges.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartImportIges.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartImportStep.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartImportStep.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartLine.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartLine.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePartPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePartPolygon.h
# End Source File
# Begin Source File

SOURCE=.\PartAlgos.cpp
# End Source File
# Begin Source File

SOURCE=.\PartAlgos.h
# End Source File
# Begin Source File

SOURCE=.\PartFeature.cpp
# End Source File
# Begin Source File

SOURCE=.\PartFeature.h
# End Source File
# Begin Source File

SOURCE=.\PartFeaturePy.cpp
# End Source File
# Begin Source File

SOURCE=.\PartFeaturePy.h
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\TopologyPy.cpp
# End Source File
# Begin Source File

SOURCE=.\TopologyPy.h
# End Source File
# End Target
# End Project
