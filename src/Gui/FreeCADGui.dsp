# Microsoft Developer Studio Project File - Name="FreeCADGui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FreeCADGui - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADGui.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADGui.mak" CFG="FreeCADGui - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FreeCADGui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FreeCADGui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "FreeCADGui___Win32_Release"
# PROP BASE Intermediate_Dir "FreeCADGui___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FREECADGUI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCGui" /YX /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(QTDIR)\lib\qt-mt230nc.lib $(QTDIR)\lib\qtmain.lib /nologo /dll /machine:I386 /out:"../../bin/FreeCADGui.dll"

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FreeCADGui___Win32_Debug"
# PROP BASE Intermediate_Dir "FreeCADGui___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FREECADGUI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCGui" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(QTDIR)\lib\qt-mt230nc.lib $(QTDIR)\lib\qtmain.lib /nologo /dll /debug /machine:I386 /out:"../../bin/FreeCADGuiD.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FreeCADGui - Win32 Release"
# Name "FreeCADGui - Win32 Debug"
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgDocTemplates.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build
InputPath=.\DlgDocTemplates.h

"DlgDocTemplates_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o DlgDocTemplates_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build
InputPath=.\DlgDocTemplates.h

"DlgDocTemplates_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o DlgDocTemplates_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgDocTemplates.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build
InputPath=.\DlgDocTemplates.ui

BuildCmds= \
	%QTDIR%\bin\uic -o DlgDocTemplates.h $(InputPath) \
	%QTDIR%\bin\uic -o DlgDocTemplates.cpp -impl DlgDocTemplates.h  $(InputPath) \
	

"DlgDocTemplates.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"DlgDocTemplates.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build
InputPath=.\DlgDocTemplates.ui

BuildCmds= \
	%QTDIR%\bin\uic -o DlgDocTemplates.h $(InputPath) \
	%QTDIR%\bin\uic -o DlgDocTemplates.cpp -impl DlgDocTemplates.h  $(InputPath) \
	

"DlgDocTemplates.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"DlgDocTemplates.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgDocTemplatesImp.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgDocTemplatesImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build
InputPath=.\DlgDocTemplatesImp.h

"DlgDocTemplatesImp_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o DlgDocTemplatesImp_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build
InputPath=.\DlgDocTemplatesImp.h

"DlgDocTemplatesImp_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o DlgDocTemplatesImp_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgUndoRedo.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgUndoRedo.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build
InputPath=.\DlgUndoRedo.h

"DlgUndoRedo_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o DlgUndoRedo_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build
InputPath=.\DlgUndoRedo.h

"DlgUndoRedo_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o DlgUndoRedo_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Application.cxx
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Application.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Application.h

"Application_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Application_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Application.h

"Application_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Application_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ButtonGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\ButtonGroup.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build
InputPath=.\ButtonGroup.h

"ButtonGroup_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o ButtonGroup_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build
InputPath=.\ButtonGroup.h

"ButtonGroup_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o ButtonGroup_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Command.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Command.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Command.h

"Command_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Command_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Command.h

"Command_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Command_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommandLine.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandLine.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build
InputPath=.\CommandLine.h

"CommandLine_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o CommandLine_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build
InputPath=.\CommandLine.h

"CommandLine_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o CommandLine_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommandStd.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\CommandStd.h
# End Source File
# Begin Source File

SOURCE=.\DllMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Document.cxx
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Document.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Document.h

"Document_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Document_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Document.h

"Document_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Document_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GuiConsole.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\GuiConsole.h
# End Source File
# Begin Source File

SOURCE=.\HtmlView.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlView.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputPath=.\HtmlView.h

"HtmlView_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o HtmlView_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputPath=.\HtmlView.h

"HtmlView_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o HtmlView_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Libs.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseModel.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\MouseModel.h
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\Themes.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Themes.h
# End Source File
# Begin Source File

SOURCE=.\Tree.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Tree.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Tree.h

"Tree_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Tree_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Tree.h

"Tree_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Tree_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\View.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\View.h

"View_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o View_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\View.h

"View_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o View_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View3D.cpp
# ADD CPP /YX"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\View3D.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\View3D.h

"View3D_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o View3D_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\View3D.h

"View3D_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o View3D_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Window.cxx
# End Source File
# Begin Source File

SOURCE=.\Window.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Window.h

"Window_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Window_moc.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Using moc on $(InputPath)
InputPath=.\Window.h

"Window_moc.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc $(InputPath) -o Window_moc.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
