# Microsoft Developer Studio Project File - Name="BuildSourceDoc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=BuildSourceDoc - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "BuildSourceDoc.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "BuildSourceDoc.mak" CFG="BuildSourceDoc - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "BuildSourceDoc - Win32 Release" (basierend auf  "Win32 (x86) Generic Project")
!MESSAGE "BuildSourceDoc - Win32 Debug" (basierend auf  "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "BuildSourceDoc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BuildSourceDoc___Win32_Release"
# PROP BASE Intermediate_Dir "BuildSourceDoc___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "BuildSourceDoc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BuildSourceDoc___Win32_Debug"
# PROP BASE Intermediate_Dir "BuildSourceDoc___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "BuildSourceDoc - Win32 Release"
# Name "BuildSourceDoc - Win32 Debug"
# Begin Source File

SOURCE=.\BuildDocDoxy.cfg

!IF  "$(CFG)" == "BuildSourceDoc - Win32 Release"

# Begin Custom Build - Extract source documentation
InputPath=.\BuildDocDoxy.cfg

"dummy.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	doxygen.exe BuildDocDoxy.cfg

# End Custom Build

!ELSEIF  "$(CFG)" == "BuildSourceDoc - Win32 Debug"

# Begin Custom Build - Extract source documentation
InputPath=.\BuildDocDoxy.cfg

"dummy.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	doxygen.exe BuildDocDoxy.cfg

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BuildDocDoxyFull.cfg

!IF  "$(CFG)" == "BuildSourceDoc - Win32 Release"

# Begin Custom Build -
InputPath=.\BuildDocDoxyFull.cfg

"dummy3.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	doxygen.exe BuildDocDoxyFull.cfg

# End Custom Build

!ELSEIF  "$(CFG)" == "BuildSourceDoc - Win32 Debug"

# Begin Custom Build -
InputPath=.\BuildDocDoxyFull.cfg

"dummy3.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	doxygen.exe BuildDocDoxyFull.cfg

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
