# Microsoft Developer Studio Project File - Name="AppTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=AppTest - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "AppTest.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppTest.mak" CFG="AppTest - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppTest - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "AppTest - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppTest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Mod/Test"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 /nologo /subsystem:console /machine:I386 /out:"../../../Mod/Test/dummy.log"

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Mod/Test"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:console /debug /machine:I386 /out:"../../../Mod/Test/dummy.log" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AppTest - Win32 Release"
# Name "AppTest - Win32 Debug"
# Begin Source File

SOURCE=.\Base.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Base.py
InputName=Base

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Base.py
InputName=Base

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Document.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Document.py
InputName=Document

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Document.py
InputName=Document

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Init.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Init.py
InputName=Init

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Init.py
InputName=Init

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InitGui.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\InitGui.py
InputName=InitGui

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\InitGui.py
InputName=InitGui

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Menu.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Menu.py
InputName=Menu

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\Menu.py
InputName=Menu

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TestApp.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\TestApp.py
InputName=TestApp

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\TestApp.py
InputName=TestApp

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TestGui.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\TestGui.py
InputName=TestGui

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\TestGui.py
InputName=TestGui

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\unittestgui.py

!IF  "$(CFG)" == "AppTest - Win32 Release"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\unittestgui.py
InputName=unittestgui

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ELSEIF  "$(CFG)" == "AppTest - Win32 Debug"

# Begin Custom Build - Copying $(InputName).py ...
InputDir=.
OutDir=.\../../../Mod/Test
InputPath=.\unittestgui.py
InputName=unittestgui

"$(OutDir)\$(InputName).py" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(InputDir)\$(InputName).py" "$(OutDir)\$(InputName).py"

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
