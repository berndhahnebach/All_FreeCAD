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
# PROP Output_Dir "../../lib/"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FREECADGUI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "." /I "inventor/Qt" /D "NDEBUG" /D "_USRDLL" /D "FCGui" /D "SCI_LEXER" /Yu"PreCompiled.h" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/FreeCADGui.dll"

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FreeCADGui___Win32_Debug"
# PROP BASE Intermediate_Dir "FreeCADGui___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../lib/"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FREECADGUI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "inventor/Qt" /D "_USRDLL" /D "FCGui" /D "FC_DEBUG" /D "SCI_LEXER" /Yu"PreCompiled.h" /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/FreeCADGuiD.dll"

!ENDIF 

# Begin Target

# Name "FreeCADGui - Win32 Release"
# Name "FreeCADGui - Win32 Debug"
# Begin Group "Dialog"

# PROP Default_Filter ""
# Begin Group "Settings"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgEditor.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgEditor.ui
InputName=DlgEditor

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgEditor.ui
InputName=DlgEditor

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgEditorImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgEditorImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgEditorImp.h
InputName=DlgEditorImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgEditorImp.h
InputName=DlgEditorImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgGeneral.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgGeneral.ui
InputName=DlgGeneral

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgGeneral.ui
InputName=DlgGeneral

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgGeneralImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgGeneralImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgGeneralImp.h
InputName=DlgGeneralImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgGeneralImp.h
InputName=DlgGeneralImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgOnlineHelp.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgOnlineHelp.ui
InputName=DlgOnlineHelp

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgOnlineHelp.ui
InputName=DlgOnlineHelp

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgOnlineHelpImp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOnlineHelpImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgOnlineHelpImp.h
InputName=DlgOnlineHelpImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgOnlineHelpImp.h
InputName=DlgOnlineHelpImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgPreferences.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgPreferences.ui
InputName=DlgPreferences

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgPreferences.ui
InputName=DlgPreferences

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgPreferencesImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgPreferencesImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgPreferencesImp.h
InputName=DlgPreferencesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgPreferencesImp.h
InputName=DlgPreferencesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgSettings.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettings.ui
InputName=DlgSettings

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettings.ui
InputName=DlgSettings

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgSettings3DView.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettings3DView.ui
InputName=DlgSettings3DView

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettings3DView.ui
InputName=DlgSettings3DView

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgSettings3DViewImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgSettings3DViewImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettings3DViewImp.h
InputName=DlgSettings3DViewImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettings3DViewImp.h
InputName=DlgSettings3DViewImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgSettingsImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgSettingsImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettingsImp.h
InputName=DlgSettingsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettingsImp.h
InputName=DlgSettingsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgSettingsMacro.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettingsMacro.ui
InputName=DlgSettingsMacro

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettingsMacro.ui
InputName=DlgSettingsMacro

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgSettingsMacroImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgSettingsMacroImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettingsMacroImp.h
InputName=DlgSettingsMacroImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettingsMacroImp.h
InputName=DlgSettingsMacroImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Customize"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DlgActions.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgActions.ui
InputName=DlgActions

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgActions.ui
InputName=DlgActions

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgActionsImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgActionsImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgActionsImp.h
InputName=DlgActionsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgActionsImp.h
InputName=DlgActionsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgCmdbars.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgCmdbars.ui
InputName=DlgCmdbars

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgCmdbars.ui
InputName=DlgCmdbars

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgCmdbarsImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgCmdbarsImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCmdbarsImp.h
InputName=DlgCmdbarsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCmdbarsImp.h
InputName=DlgCmdbarsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgCommands.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgCommands.ui
InputName=DlgCommands

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgCommands.ui
InputName=DlgCommands

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgCommandsImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgCommandsImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCommandsImp.h
InputName=DlgCommandsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCommandsImp.h
InputName=DlgCommandsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgCustomizeImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCustomizeImp.h
InputName=DlgCustomizeImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCustomizeImp.h
InputName=DlgCustomizeImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgToolbars.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgToolbars.ui
InputName=DlgToolbars

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgToolbars.ui
InputName=DlgToolbars

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgToolbarsImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgToolbarsImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgToolbarsImp.h
InputName=DlgToolbarsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgToolbarsImp.h
InputName=DlgToolbarsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\DlgDocTemplates.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgDocTemplates.ui
InputName=DlgDocTemplates

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgDocTemplates.ui
InputName=DlgDocTemplates

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgDocTemplatesImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgDocTemplatesImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgDocTemplatesImp.h
InputName=DlgDocTemplatesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgDocTemplatesImp.h
InputName=DlgDocTemplatesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgMacroExecute.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgMacroExecute.ui
InputName=DlgMacroExecute

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgMacroExecute.ui
InputName=DlgMacroExecute

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgMacroExecuteImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgMacroExecuteImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgMacroExecuteImp.h
InputName=DlgMacroExecuteImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgMacroExecuteImp.h
InputName=DlgMacroExecuteImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgMacroRecord.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgMacroRecord.ui
InputName=DlgMacroRecord

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgMacroRecord.ui
InputName=DlgMacroRecord

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgMacroRecordImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgMacroRecordImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgMacroRecordImp.h
InputName=DlgMacroRecordImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgMacroRecordImp.h
InputName=DlgMacroRecordImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgParameter.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgParameter.ui
InputName=DlgParameter

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgParameter.ui
InputName=DlgParameter

BuildCmds= \
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h \
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp \
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgParameterImp.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgParameterImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgParameterImp.h
InputName=DlgParameterImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgParameterImp.h
InputName=DlgParameterImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgUndoRedo.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\DlgUndoRedo.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgUndoRedo.h
InputName=DlgUndoRedo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgUndoRedo.h
InputName=DlgUndoRedo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PropertyPage.h
# End Source File
# End Group
# Begin Group "Command"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Command.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Command.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Command.h
InputName=Command

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Command.h
InputName=Command

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CommandStd.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\CommandTest.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\CommandView.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# End Group
# Begin Group "Dock windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ButtonGroup.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\ButtonGroup.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ButtonGroup.h
InputName=ButtonGroup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ButtonGroup.h
InputName=ButtonGroup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HtmlView.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\HtmlView.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\HtmlView.h
InputName=HtmlView

BuildCmds= \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\moc_private_$(InputName).cpp \
	

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_private_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\HtmlView.h
InputName=HtmlView

BuildCmds= \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\moc_private_$(InputName).cpp \
	

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_private_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HtmlViewP.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\HtmlViewP.h
InputName=HtmlViewP

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\HtmlViewP.h
InputName=HtmlViewP

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReportView.cpp
# End Source File
# Begin Source File

SOURCE=.\ReportView.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ReportView.h
InputName=ReportView

BuildCmds= \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\moc_private_$(InputName).cpp \
	

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_private_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ReportView.h
InputName=ReportView

BuildCmds= \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp \
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).cpp -o $(InputDir)\moc_private_$(InputName).cpp \
	

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_private_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Scintilla"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scintilla\AutoComplete.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\AutoComplete.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\CallTip.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\CallTip.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\CellBuffer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\CellBuffer.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\ContractionState.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\ContractionState.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\DocumentAccessor.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\DocumentAccessor.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\Documents.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\Documents.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\Editor.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\Editor.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\ExternalLexer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\ExternalLexer.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\Indicator.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\Indicator.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\KeyMap.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\KeyMap.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\KeyWords.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexAda.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexAsm.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexAVE.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexBaan.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexBullant.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexConf.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexCPP.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexCrontab.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexCSS.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexEiffel.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexEScript.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexFortran.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexHTML.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexLisp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexLout.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexLua.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexMatlab.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexOthers.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexPascal.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexPerl.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexPOV.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexPython.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexRuby.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexSQL.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LexVB.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LineMarker.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\LineMarker.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\PropSet.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\RESearch.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\RESearch.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\ScintillaBase.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\ScintillaBase.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\Style.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\Style.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\StyleContext.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\StyleContext.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\SVector.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\UniConversion.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\UniConversion.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\ViewStyle.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\ViewStyle.h
# End Source File
# Begin Source File

SOURCE=.\scintilla\WindowAccessor.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\XPM.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\scintilla\XPM.h
# End Source File
# End Group
# Begin Group "qextmdi"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\qextmdi\dummy_moc.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\dummykmainwindow.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\qextmdi
InputPath=.\qextmdi\dummykmainwindow.h
InputName=dummykmainwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\qextmdi
InputPath=.\qextmdi\dummykmainwindow.h
InputName=dummykmainwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\dummykpartsdockmainwindow.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\qextmdi
InputPath=.\qextmdi\dummykpartsdockmainwindow.h
InputName=dummykpartsdockmainwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\qextmdi
InputPath=.\qextmdi\dummykpartsdockmainwindow.h
InputName=dummykpartsdockmainwindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\dummyktoolbar.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\qextmdi
InputPath=.\qextmdi\dummyktoolbar.h
InputName=dummyktoolbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\qextmdi
InputPath=.\qextmdi\dummyktoolbar.h
InputName=dummyktoolbar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\exportdockclass.h
# End Source File
# Begin Source File

SOURCE=.\qextmdi\kdocktabctl.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\kdocktabctl.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\kdocktabctl.h
InputName=kdocktabctl

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\kdocktabctl.h
InputName=kdocktabctl

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\kdockwidget.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\kdockwidget.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\kdockwidget.h
InputName=kdockwidget

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\kdockwidget.h
InputName=kdockwidget

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\kdockwidget_private.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\kdockwidget_private.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\kdockwidget_private.h
InputName=kdockwidget_private

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\kdockwidget_private.h
InputName=kdockwidget_private

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildarea.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildarea.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildarea.h
InputName=qextmdichildarea

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildarea.h
InputName=qextmdichildarea

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildfrm.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildfrm.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildfrm.h
InputName=qextmdichildfrm

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildfrm.h
InputName=qextmdichildfrm

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildfrmcaption.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildfrmcaption.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildfrmcaption.h
InputName=qextmdichildfrmcaption

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildfrmcaption.h
InputName=qextmdichildfrmcaption

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildview.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdichildview.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildview.h
InputName=qextmdichildview

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdichildview.h
InputName=qextmdichildview

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdidefines.h
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdiiterator.h
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdilistiterator.h
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdimainfrm.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdimainfrm.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdimainfrm.h
InputName=qextmdimainfrm

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc 
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\post_moc.py $(InputDir)\$(InputName).moc 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmdimainfrm.h
InputName=qextmdimainfrm

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc 
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\post_moc.py $(InputDir)\$(InputName).moc 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmdinulliterator.h
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmditaskbar.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\qextmdi\qextmditaskbar.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmditaskbar.h
InputName=qextmditaskbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\qextmdi
InputPath=.\qextmdi\qextmditaskbar.h
InputName=qextmditaskbar

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\$(InputName).moc

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Inventor"

# PROP Default_Filter ""
# Begin Group "OCC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Inventor\OCC\SoBrepShape.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Inventor\OCC\SoBrepShape.h
# End Source File
# End Group
# End Group
# Begin Group "Language"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Language\FreeCAD_de.ts

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Building $(InputName).h
InputDir=.\Language
InputPath=.\Language\FreeCAD_de.ts
InputName=FreeCAD_de

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\tstocpp.py $(InputDir)\$(InputName).ts $(InputDir)\$(InputName).h $(InputName) 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Building $(InputName).h
InputDir=.\Language
InputPath=.\Language\FreeCAD_de.ts
InputName=FreeCAD_de

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\tstocpp.py $(InputDir)\$(InputName).ts $(InputDir)\$(InputName).h $(InputName) 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Language\LanguageFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Language\LanguageFactory.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Application.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Application.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Application.h
InputName=Application

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Application.h
InputName=Application

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BitmapFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapFactory.h
# End Source File
# Begin Source File

SOURCE=.\CommandLine.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\CommandLine.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\CommandLine.h
InputName=CommandLine

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\CommandLine.h
InputName=CommandLine

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DllMain.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Document.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Document.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Document.h
InputName=Document

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Document.h
InputName=Document

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeCADGuiInit.py

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Building InitGuiScript.h
InputPath=.\FreeCADGuiInit.py

"GuiInitScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\PythonToCPP.py FreeCADGuiInit.py GuiInitScript.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Building InitGuiScript.h
InputPath=.\FreeCADGuiInit.py

"GuiInitScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\PythonToCPP.py FreeCADGuiInit.py GuiInitScript.h 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GuiConsole.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\GuiConsole.h
# End Source File
# Begin Source File

SOURCE=.\Libs.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Macro.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Macro.h
# End Source File
# Begin Source File

SOURCE=.\MouseModel.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\MouseModel.h
# End Source File
# Begin Source File

SOURCE=.\PlatQt.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PlatQt.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PlatQt.h
InputName=PlatQt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PlatQt.h
InputName=PlatQt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# Begin Source File

SOURCE=.\PrefWidgets.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PrefWidgets.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PrefWidgets.h
InputName=PrefWidgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PrefWidgets.h
InputName=PrefWidgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Process.cpp
# End Source File
# Begin Source File

SOURCE=.\Process.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Process.h
InputName=Process

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Process.h
InputName=Process

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProcessQt.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ProcessQt.h
InputName=ProcessQt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ProcessQt.h
InputName=ProcessQt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertyView.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PropertyView.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PropertyView.h
InputName=PropertyView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PropertyView.h
InputName=PropertyView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\SciEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\SciEditor.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\SciEditor.h
InputName=SciEditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\SciEditor.h
InputName=SciEditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScintillaQt.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\ScintillaQt.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ScintillaQt.h
InputName=ScintillaQt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ScintillaQt.h
InputName=ScintillaQt

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Selection.cpp
# End Source File
# Begin Source File

SOURCE=.\Selection.h
# End Source File
# Begin Source File

SOURCE=.\Splashscreen.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Splashscreen.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Splashscreen.h
InputName=Splashscreen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Splashscreen.h
InputName=Splashscreen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Themes.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Themes.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Themes.h
InputName=Themes

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Themes.h
InputName=Themes

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tools.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Tools.h
# End Source File
# Begin Source File

SOURCE=.\Tree.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Tree.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Tree.h
InputName=Tree

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Tree.h
InputName=Tree

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\View.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View.h
InputName=View

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View.h
InputName=View

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View3D.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\View3D.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View3D.h
InputName=View3D

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View3D.h
InputName=View3D

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View3DInventor.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\View3DInventor.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View3DInventor.h
InputName=View3DInventor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View3DInventor.h
InputName=View3DInventor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ViewProvider.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewProvider.h
# End Source File
# Begin Source File

SOURCE=.\WidgetFactory.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\WidgetFactory.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\WidgetFactory.h
InputName=WidgetFactory

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\WidgetFactory.h
InputName=WidgetFactory

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Widgets.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Widgets.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Widgets.h
InputName=Widgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Widgets.h
InputName=Widgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Window.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Window.h
InputName=Window

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Window.h
InputName=Window

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
