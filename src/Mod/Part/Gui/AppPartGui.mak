# Microsoft Developer Studio Generated NMAKE File, Based on AppPartGui.dsp
!IF "$(CFG)" == ""
CFG=AppPartGui - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. AppPartGui - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "AppPartGui - Win32 Release" && "$(CFG)" != "AppPartGui - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppPartGui.mak" CFG="AppPartGui - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppPartGui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppPartGui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR Eine ungÅltige Konfiguration wurde angegeben.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : 


CLEAN :
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPPARTGUI_EXPORTS" /Fp"$(INTDIR)\AppPartGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPartGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AppPartGui.pdb" /machine:I386 /out:"$(OUTDIR)\AppPartGui.dll" /implib:"$(OUTDIR)\AppPartGui.lib" 
LINK32_OBJS= \
	

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : 


CLEAN :
	-@erase 

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPPARTGUI_EXPORTS" /Fp"$(INTDIR)\AppPartGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPartGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\AppPartGui.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AppPartGui.dll" /implib:"$(OUTDIR)\AppPartGui.lib" /pdbtype:sept 
LINK32_OBJS= \
	

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("AppPartGui.dep")
!INCLUDE "AppPartGui.dep"
!ELSE 
!MESSAGE Warning: cannot find "AppPartGui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AppPartGui - Win32 Release" || "$(CFG)" == "AppPartGui - Win32 Debug"

!ENDIF 

