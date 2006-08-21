# Microsoft Developer Studio Project File - Name="BuildExtractRevision" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=BuildExtractRevision - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "BuildExtractRevision.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "BuildExtractRevision.mak" CFG="BuildExtractRevision - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "BuildExtractRevision - Win32 Release" (basierend auf  "Win32 (x86) Generic Project")
!MESSAGE "BuildExtractRevision - Win32 Debug" (basierend auf  "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "BuildExtractRevision - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "BuildExtractRevision___Win32_Release"
# PROP BASE Intermediate_Dir "BuildExtractRevision___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "BuildExtractRevision - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "BuildExtractRevision___Win32_Debug"
# PROP BASE Intermediate_Dir "BuildExtractRevision___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "BuildExtractRevision - Win32 Release"
# Name "BuildExtractRevision - Win32 Debug"
# Begin Source File

SOURCE=.\Version.h.in

!IF  "$(CFG)" == "BuildExtractRevision - Win32 Release"

# Begin Custom Build - Extract revision number...
InputPath=.\Version.h.in

"dummy.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SubWCRev.exe .. Version.h.in Version.h

# End Custom Build

!ELSEIF  "$(CFG)" == "BuildExtractRevision - Win32 Debug"

# Begin Custom Build - Extract revision number...
InputPath=.\Version.h.in

"dummy.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SubWCRev.exe .. Version.h.in Version.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Version.wxi.in

!IF  "$(CFG)" == "BuildExtractRevision - Win32 Release"

# Begin Custom Build
InputPath=.\Version.wxi.in

"dummy2.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SubWCRev.exe .. Version.wxi.in Version.wxi

# End Custom Build

!ELSEIF  "$(CFG)" == "BuildExtractRevision - Win32 Debug"

# Begin Custom Build
InputPath=.\Version.wxi.in

"dummy2.txt" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	SubWCRev.exe .. Version.wxi.in Version.wxi

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
