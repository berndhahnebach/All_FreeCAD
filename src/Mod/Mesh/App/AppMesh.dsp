# Microsoft Developer Studio Project File - Name="AppMesh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AppMesh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AppMesh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AppMesh.mak" CFG="AppMesh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AppMesh - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppMesh - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppMesh - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPMesh_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../" /D "NDEBUG" /D "_USRDLL" /D "FCAppMesh" /Yu"PreCompiled.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release\Mesh.pyd" /libpath:"../../../../lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\..\Mod\Mesh	copy Release\Mesh.pyd ..\..\..\..\Mod\Mesh	copy ..\Init.py ..\..\..\..\Mod\Mesh
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AppMesh - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPMesh_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../" /D "_USRDLL" /D "FCAppMesh" /D "FC_DEBUG" /FR /Yu"PreCompiled.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug\Mesh_d.pyd" /pdbtype:sept /libpath:"../../../../lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\..\..\Mod\Mesh	copy Debug\Mesh_d.pyd ..\..\..\..\Mod\Mesh	copy ..\Init.py ..\..\..\..\Mod\Mesh
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AppMesh - Win32 Release"
# Name "AppMesh - Win32 Debug"
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core\Algorithm.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Algorithm.h
# End Source File
# Begin Source File

SOURCE=.\Core\Approximation.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Approximation.h
# End Source File
# Begin Source File

SOURCE=.\Core\BoundBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\BoundBox.h
# End Source File
# Begin Source File

SOURCE=.\Core\Definitions.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Definitions.h
# End Source File
# Begin Source File

SOURCE=.\Core\Degeneration.cpp

!IF  "$(CFG)" == "AppMesh - Win32 Release"

!ELSEIF  "$(CFG)" == "AppMesh - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Core\Degeneration.h
# End Source File
# Begin Source File

SOURCE=.\Core\Elements.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Elements.h
# End Source File
# Begin Source File

SOURCE=.\Core\Evaluation.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Evaluation.h
# End Source File
# Begin Source File

SOURCE=.\Core\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Grid.h
# End Source File
# Begin Source File

SOURCE=.\Core\Helpers.h
# End Source File
# Begin Source File

SOURCE=.\Core\Info.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Info.h
# End Source File
# Begin Source File

SOURCE=.\Core\Iterator.h
# End Source File
# Begin Source File

SOURCE=.\Core\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Core\MeshAlgorithm.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MeshAlgorithm.h
# End Source File
# Begin Source File

SOURCE=.\Core\MeshIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MeshIO.h
# End Source File
# Begin Source File

SOURCE=.\Core\MeshKernel.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MeshKernel.h
# End Source File
# Begin Source File

SOURCE=.\Core\Stream.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Stream.h
# End Source File
# Begin Source File

SOURCE=.\Core\Swap.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Swap.h
# End Source File
# Begin Source File

SOURCE=.\Core\Tools.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Tools.h
# End Source File
# Begin Source File

SOURCE=.\Core\Tools2D.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Tools2D.h
# End Source File
# Begin Source File

SOURCE=.\Core\TopoAlgorithm.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\TopoAlgorithm.h
# End Source File
# Begin Source File

SOURCE=.\Core\Vector3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Vector3D.h
# End Source File
# Begin Source File

SOURCE=.\Core\ViewProj.h
# End Source File
# Begin Source File

SOURCE=.\Core\Visitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Visitor.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AppMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\AppMeshPy.cpp
# End Source File
# Begin Source File

SOURCE=.\Doxygen.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\FeatureMeshImport.cpp
# End Source File
# Begin Source File

SOURCE=.\FeatureMeshImport.h
# End Source File
# Begin Source File

SOURCE=.\Libs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Mesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Mesh.h
# End Source File
# Begin Source File

SOURCE=.\MeshAlgos.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshAlgos.h
# End Source File
# Begin Source File

SOURCE=.\MeshCurvature.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshCurvature.h
# End Source File
# Begin Source File

SOURCE=.\MeshFeature.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshFeature.h
# End Source File
# Begin Source File

SOURCE=.\MeshFeaturePy.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshFeaturePy.h
# End Source File
# Begin Source File

SOURCE=.\MeshPy.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshPy.h
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# End Target
# End Project
