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
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp" /YX /FD /c
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
# ADD LINK32 TKPCAF.lib winspool.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /machine:I386 /out:"../../bin/FreeCADApp.dll"

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
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /D "_USRDLL" /D "FCApp" /D "FC_DEBUG" /FR /YX"PreCompiled.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winspool.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /dll /debug /machine:I386 /out:"../../bin/FreeCADAppD.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FreeCADApp - Win32 Release"
# Name "FreeCADApp - Win32 Debug"
# Begin Source File

SOURCE=.\Application.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yu"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=.\Attribute.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yu"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Attribute.h
# End Source File
# Begin Source File

SOURCE=.\DllMain.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Document.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yu"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Document.h
# End Source File
# Begin Source File

SOURCE=.\Geometry2D.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yu"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Geometry2D.h
# End Source File
# Begin Source File

SOURCE=.\Geometry3D.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yu"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Geometry3D.h
# End Source File
# Begin Source File

SOURCE=.\Libs.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yc"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\Topology.cpp

!IF  "$(CFG)" == "FreeCADApp - Win32 Release"

!ELSEIF  "$(CFG)" == "FreeCADApp - Win32 Debug"

# ADD CPP /Yu"PreCompiled.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Topology.h
# End Source File
# End Target
# End Project
