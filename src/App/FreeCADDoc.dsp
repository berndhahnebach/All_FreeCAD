# Microsoft Developer Studio Project File - Name="FreeCADApp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FreeCADApp - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADDoc.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADDoc.mak" CFG="FreeCADApp - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FreeCADApp - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FreeCADApp - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FreeCADApp_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I ".." /D "NDEBUG" /D "_USRDLL" /D "FCApp" /Yu"PreCompiled.h" /FD /c
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
# ADD LINK32 MSVCRT.LIB MSVCPRT.LIB kernel32.lib xerces-c_2.lib python23.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release/FreeCADApp.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy           /y           /f           Release\FreeCADApp.dll           ..\..\bin\          	xcopy           /y           /f           Release\FreeCADApp.lib           ..\..\lib\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FreeCADApp___Win32_Debug"
# PROP BASE Intermediate_Dir "FreeCADApp___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FreeCADApp_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /I ".." /D "_USRDLL" /D "FCApp" /D "FC_DEBUG" /Yu"PreCompiled.h" /FD /GZ /c
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
# ADD LINK32 MSVCRTD.LIB MSVCPRTD.LIB kernel32.lib xerces-c_2D.lib python23_d.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug/FreeCADAppD.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy           /y           /f           Debug\FreeCADAppD.dll           ..\..\bin\          	xcopy           /y           /f           Debug\FreeCADAppD.lib           ..\..\lib\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FreeCADApp - Win32 Release"
# Name "FreeCADApp - Win32 Debug"
# Begin Group "Properties"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Property.cpp
# End Source File
# Begin Source File

SOURCE=.\Property.h
# End Source File
# Begin Source File

SOURCE=.\PropertyContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyContainer.h
# End Source File
# Begin Source File

SOURCE=.\PropertyGeo.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyGeo.h
# End Source File
# Begin Source File

SOURCE=.\PropertyLinks.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyLinks.h
# End Source File
# Begin Source File

SOURCE=.\PropertyStandard.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyStandard.h
# End Source File
# End Group
# Begin Group "Document"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Document.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Document.h
# End Source File
# Begin Source File

SOURCE=.\DocumentObject.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentObject.h
# End Source File
# Begin Source File

SOURCE=.\DocumentObjectGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentObjectGroup.h
# End Source File
# Begin Source File

SOURCE=.\DocumentObjectGroupPy.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentObjectGroupPy.h
# End Source File
# Begin Source File

SOURCE=.\DocumentObjectPy.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentObjectPy.h
# End Source File
# Begin Source File

SOURCE=.\DocumentPy.cpp
# End Source File
# Begin Source File

SOURCE=.\DocumentPy.h
# End Source File
# Begin Source File

SOURCE=.\Feature.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Feature.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePy.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePy.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePython.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePython.h
# End Source File
# Begin Source File

SOURCE=.\FeaturePythonPy.cpp
# End Source File
# Begin Source File

SOURCE=.\FeaturePythonPy.h
# End Source File
# Begin Source File

SOURCE=.\FeatureTest.cpp
# End Source File
# Begin Source File

SOURCE=.\FeatureTest.h
# End Source File
# Begin Source File

SOURCE=.\GeoFeature.cpp
# End Source File
# Begin Source File

SOURCE=.\GeoFeature.h
# End Source File
# Begin Source File

SOURCE=.\Placement.cpp
# End Source File
# Begin Source File

SOURCE=.\Placement.h
# End Source File
# Begin Source File

SOURCE=.\Transactions.cpp
# End Source File
# Begin Source File

SOURCE=.\Transactions.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Application.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=.\ApplicationPy.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorModel.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorModel.h
# End Source File
# Begin Source File

SOURCE=.\DataWithProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\DataWithProperty.h
# End Source File
# Begin Source File

SOURCE=.\Doxygen.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\FreeCADInit.py

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

# Begin Custom Build - Building InitScript.h
InputPath=.\FreeCADInit.py

"InitScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADInit.py InitScript.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# Begin Custom Build - Building InitScript.h
InputPath=.\FreeCADInit.py

"InitScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADInit.py InitScript.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeCADTest.py

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

# Begin Custom Build - Building TestScript.h
InputPath=.\FreeCADTest.py

"TestScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADTest.py TestScript.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# Begin Custom Build - Building TestScript.h
InputPath=.\FreeCADTest.py

"TestScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADTest.py TestScript.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Geometry2D.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Geometry2D.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Geometry3D.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Geometry3D.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Material.cpp
# End Source File
# Begin Source File

SOURCE=.\Material.h
# End Source File
# Begin Source File

SOURCE=.\MaterialPy.cpp
# End Source File
# Begin Source File

SOURCE=.\MaterialPy.h
# End Source File
# Begin Source File

SOURCE=.\MatrixPy.cpp
# End Source File
# Begin Source File

SOURCE=.\MatrixPy.h
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\VectorPy.cpp
# End Source File
# Begin Source File

SOURCE=.\VectorPy.h
# End Source File
# End Target
# End Project
