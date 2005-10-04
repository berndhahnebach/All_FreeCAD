# Microsoft Developer Studio Project File - Name="FreeCADGui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FreeCADGui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADGui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADGui.mak" CFG="FreeCADGui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FreeCADGui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FreeCADGui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /O2 /I "." /I "inventor/Qt" /I ".." /D "NDEBUG" /D "_USRDLL" /D "FCGui" /D "SCI_LEXER" /Yu"PreCompiled.h" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\bin	copy Release\FreeCADGui.dll ..\..\bin	mkdir ..\..\lib	copy Release\FreeCADGui.lib ..\..\lib
# End Special Build Tool

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
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "inventor/Qt" /I ".." /D "_USRDLL" /D "FCGui" /D "FC_DEBUG" /D "SCI_LEXER" /Yu"PreCompiled.h" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/FreeCADGuiD.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=mkdir ..\..\bin	copy Debug\FreeCADGuiD.dll ..\..\bin	mkdir ..\..\lib	copy Debug\FreeCADGuiD.lib ..\..\lib
# End Special Build Tool

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgEditorImp.h
InputName=DlgEditorImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgGeneralImp.h
InputName=DlgGeneralImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgOnlineHelpImp.h
InputName=DlgOnlineHelpImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgPreferencesImp.h
InputName=DlgPreferencesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgReportView.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgReportView.ui
InputName=DlgReportView

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
InputPath=.\DlgReportView.ui
InputName=DlgReportView

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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

SOURCE=.\DlgReportViewImp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReportViewImp.h
# End Source File
# Begin Source File

SOURCE=.\DlgSettings3DView.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgSettings3DView.ui
InputName=DlgSettings3DView

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettings3DViewImp.h
InputName=DlgSettings3DViewImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgSettingsMacroImp.h
InputName=DlgSettingsMacroImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgActionsImp.h
InputName=DlgActionsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCmdbarsImp.h
InputName=DlgCmdbarsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCommandsImp.h
InputName=DlgCommandsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgCustomizeImp.h
InputName=DlgCustomizeImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgToolbarsImp.h
InputName=DlgToolbarsImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\DlgActivateWindow.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgActivateWindow.ui
InputName=DlgActivateWindow

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
InputPath=.\DlgActivateWindow.ui
InputName=DlgActivateWindow

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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

SOURCE=.\DlgActivateWindowImp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgActivateWindowImp.h
# End Source File
# Begin Source File

SOURCE=.\DlgAuthorization.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgAuthorization.ui
InputName=DlgAuthorization

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
InputPath=.\DlgAuthorization.ui
InputName=DlgAuthorization

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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

SOURCE=.\DlgDisplayProperties.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgDisplayProperties.ui
InputName=DlgDisplayProperties

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
InputPath=.\DlgDisplayProperties.ui
InputName=DlgDisplayProperties

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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

SOURCE=.\DlgDisplayPropertiesImp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgDisplayPropertiesImp.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgDisplayPropertiesImp.h
InputName=DlgDisplayPropertiesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgDisplayPropertiesImp.h
InputName=DlgDisplayPropertiesImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgMacroExecuteImp.h
InputName=DlgMacroExecuteImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgMacroRecordImp.h
InputName=DlgMacroRecordImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgParameterImp.h
InputName=DlgParameterImp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DlgTipOfTheDay.ui

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\DlgTipOfTheDay.ui
InputName=DlgTipOfTheDay

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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
InputPath=.\DlgTipOfTheDay.ui
InputName=DlgTipOfTheDay

BuildCmds= \
	%qtdir%\bin\uic.exe "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	%qtdir%\bin\uic.exe "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	%qtdir%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

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

SOURCE=.\DlgTipOfTheDayImp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTipOfTheDayImp.h
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DlgUndoRedo.h
InputName=DlgUndoRedo

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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

SOURCE=.\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\Action.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Action.h
InputName=Action

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Action.h
InputName=Action

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Command.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Command.h
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
# Begin Source File

SOURCE=.\CommandWindow.cpp
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ButtonGroup.h
InputName=ButtonGroup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HelpView.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\HelpView.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\HelpView.h
InputName=HelpView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\HelpView.h
InputName=HelpView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PropertyView.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PropertyView.h
# End Source File
# Begin Source File

SOURCE=.\PythonConsole.cpp
# End Source File
# Begin Source File

SOURCE=.\PythonConsole.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PythonConsole.h
InputName=PythonConsole

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PythonConsole.h
InputName=PythonConsole

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PythonConsolePy.cpp
# End Source File
# Begin Source File

SOURCE=.\PythonConsolePy.h
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

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ReportView.h
InputName=ReportView

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StackBar.cpp
# End Source File
# Begin Source File

SOURCE=.\StackBar.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\StackBar.h
InputName=StackBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\StackBar.h
InputName=StackBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ToolBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBox.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ToolBox.h
InputName=ToolBox

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ToolBox.h
InputName=ToolBox

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ToolBoxBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBoxBar.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ToolBoxBar.h
InputName=ToolBoxBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ToolBoxBar.h
InputName=ToolBoxBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	python ..\Tools\tstocpp.py "$(InputDir)\$(InputName).ts" "$(InputDir)\$(InputName).h" "$(InputName)" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Building $(InputName).h
InputDir=.\Language
InputPath=.\Language\FreeCAD_de.ts
InputName=FreeCAD_de

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\tstocpp.py "$(InputDir)\$(InputName).ts" "$(InputDir)\$(InputName).h" "$(InputName)" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Language\LanguageFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\Language\LanguageFactory.h
# End Source File
# Begin Source File

SOURCE=.\Language\qt_de.ts

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Building $(InputName).h
InputDir=.\Language
InputPath=.\Language\qt_de.ts
InputName=qt_de

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\tstocpp.py "$(InputDir)\$(InputName).ts" "$(InputDir)\$(InputName).h" "$(InputName)" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Building $(InputName).h
InputDir=.\Language
InputPath=.\Language\qt_de.ts
InputName=qt_de

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist %FREECADLIB% (set PYTHONPATH=%FREECADLIB%\res\pylibs) 
	python ..\Tools\tstocpp.py "$(InputDir)\$(InputName).ts" "$(InputDir)\$(InputName).h" "$(InputName)" 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Language\Translator.cpp
# End Source File
# Begin Source File

SOURCE=.\Language\Translator.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\Language
InputPath=.\Language\Translator.h
InputName=Translator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\Language
InputPath=.\Language\Translator.h
InputName=Translator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Propertyeditor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\propertyeditor\knuminput.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\knuminput.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\propertyeditor
InputPath=.\propertyeditor\knuminput.h
InputName=knuminput

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\$(InputName).moc"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\propertyeditor
InputPath=.\propertyeditor\knuminput.h
InputName=knuminput

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\$(InputName).moc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\knumvalidator.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\knumvalidator.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\propertyeditor
InputPath=.\propertyeditor\knumvalidator.h
InputName=knumvalidator

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\$(InputName).moc"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Mocing $(InputPath)
InputDir=.\propertyeditor
InputPath=.\propertyeditor\knumvalidator.h
InputName=knumvalidator

"$(InputDir)\$(InputName).moc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\$(InputName).moc"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditor.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditor.h
InputName=propertyeditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditor.h
InputName=propertyeditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditordate.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditordate.h
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorfile.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorfile.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditorfile.h
InputName=propertyeditorfile

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditorfile.h
InputName=propertyeditorfile

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorfont.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorfont.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditorfont.h
InputName=propertyeditorfont

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditorfont.h
InputName=propertyeditorfont

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorinput.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorinput.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditorinput.h
InputName=propertyeditorinput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditorinput.h
InputName=propertyeditorinput

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditoritem.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditoritem.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditoritem.h
InputName=propertyeditoritem

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\propertyeditor
InputPath=.\propertyeditor\propertyeditoritem.h
InputName=propertyeditoritem

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorlist.cpp
# End Source File
# Begin Source File

SOURCE=.\propertyeditor\propertyeditorlist.h
# End Source File
# End Group
# Begin Group "Widget"

# PROP Default_Filter ""
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\CommandLine.h
InputName=CommandLine

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CustomWidgets.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomWidgets.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\CustomWidgets.h
InputName=CustomWidgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\CustomWidgets.h
InputName=CustomWidgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FileDialog.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\FileDialog.h
InputName=FileDialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\FileDialog.h
InputName=FileDialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWindow.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\MainWindow.h
InputName=MainWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\MainWindow.h
InputName=MainWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PrefWidgets.h
InputName=PrefWidgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ProgressBar.h
InputName=ProgressBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\ProgressBar.h
InputName=ProgressBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PythonEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\PythonEditor.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PythonEditor.h
InputName=PythonEditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\PythonEditor.h
InputName=PythonEditor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SpinBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SpinBox.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\SpinBox.h
InputName=SpinBox

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\SpinBox.h
InputName=SpinBox

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Splashscreen.h
InputName=Splashscreen

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TextEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\TextEdit.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\TextEdit.h
InputName=TextEdit

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\TextEdit.h
InputName=TextEdit

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\WidgetFactory.h
InputName=WidgetFactory

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Widgets.h
InputName=Widgets

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# ADD CPP /Yu"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# End Group
# Begin Group "View3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SoFCSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\SoFCSelection.h
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View3DInventor.h
InputName=View3DInventor

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\View3DInventorExamples.cpp
# End Source File
# Begin Source File

SOURCE=.\View3DInventorExamples.h
# End Source File
# Begin Source File

SOURCE=.\View3DInventorViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\View3DInventorViewer.h
# End Source File
# Begin Source File

SOURCE=.\ViewProvider.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewProvider.h
# End Source File
# Begin Source File

SOURCE=.\ViewProviderFeature.cpp
# End Source File
# Begin Source File

SOURCE=.\ViewProviderFeature.h
# End Source File
# End Group
# Begin Group "Workbench"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CommandBarManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandBarManager.h
# End Source File
# Begin Source File

SOURCE=.\DockWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DockWindow.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DockWindow.h
InputName=DockWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\DockWindow.h
InputName=DockWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MenuManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MenuManager.h
# End Source File
# Begin Source File

SOURCE=.\ToolBarManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolBarManager.h
# End Source File
# Begin Source File

SOURCE=.\Workbench.cpp
# End Source File
# Begin Source File

SOURCE=.\Workbench.h
# End Source File
# Begin Source File

SOURCE=.\WorkbenchFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkbenchFactory.h
# End Source File
# Begin Source File

SOURCE=.\WorkbenchManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkbenchManager.h
# End Source File
# Begin Source File

SOURCE=.\WorkbenchPy.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkbenchPy.h
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Application.h
InputName=Application

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ApplicationPy.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapFactory.h
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Document.h
InputName=Document

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Doxygen.cpp
# SUBTRACT CPP /YX /Yc /Yu
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

SOURCE=.\NetworkRetriever.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkRetriever.h

!IF  "$(CFG)" == "FreeCADGui - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\NetworkRetriever.h
InputName=NetworkRetriever

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\NetworkRetriever.h
InputName=NetworkRetriever

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Process.h
InputName=Process

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\Selection.cpp
# End Source File
# Begin Source File

SOURCE=.\Selection.h
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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\Tree.h
InputName=Tree

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

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
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADGui - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.
InputPath=.\View.h
InputName=View

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	%QTDIR%\bin\moc.exe "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WaitCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\WaitCursor.h
# End Source File
# Begin Source File

SOURCE=.\WhatsThis.cpp
# End Source File
# Begin Source File

SOURCE=.\WhatsThis.h
# End Source File
# End Target
# End Project
