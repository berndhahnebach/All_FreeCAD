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
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\AppPartGui.dll" "$(OUTDIR)\AppPartGui.pch"

!ELSE 

ALL : "AppPart - Win32 Release" "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "FreeCADGui - Win32 Release" "..\AppPartGui.dll" "$(OUTDIR)\AppPartGui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 ReleaseCLEAN" "FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" "AppPart - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPartGui.obj"
	-@erase "$(INTDIR)\AppPartGui.pch"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\DlgPartBoxImp.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AppPartGui.exp"
	-@erase "$(OUTDIR)\AppPartGui.lib"
	-@erase "..\AppPartGui.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPartGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AppPartGui.pdb" /machine:I386 /out:"../AppPartGui.dll" /implib:"$(OUTDIR)\AppPartGui.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AppPartGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgPartBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Release\FreeCADGui.lib" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib" \
	"..\AppPart.lib"

"..\AppPartGui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\AppPartGuiD.dll" "$(OUTDIR)\AppPartGui.pch"

!ELSE 

ALL : "AppPart - Win32 Debug" "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "FreeCADGui - Win32 Debug" "..\AppPartGuiD.dll" "$(OUTDIR)\AppPartGui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 DebugCLEAN" "FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" "AppPart - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPartGui.obj"
	-@erase "$(INTDIR)\AppPartGui.pch"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\DlgPartBoxImp.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AppPartGuiD.exp"
	-@erase "$(OUTDIR)\AppPartGuiD.lib"
	-@erase "$(OUTDIR)\AppPartGuiD.pdb"
	-@erase "..\AppPartGuiD.dll"
	-@erase "..\AppPartGuiD.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPartGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\AppPartGuiD.pdb" /debug /machine:I386 /out:"../AppPartGuiD.dll" /implib:"$(OUTDIR)\AppPartGuiD.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AppPartGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgPartBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Debug\FreeCADGuiD.lib" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib" \
	"..\AppPartD.lib"

"..\AppPartGuiD.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

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

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"FreeCADGui - Win32 Release" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADGui.mak" CFG="FreeCADGui - Win32 Release" 
   cd "..\MOD\PART\GUI"

"FreeCADGui - Win32 ReleaseCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADGui.mak" CFG="FreeCADGui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\PART\GUI"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"FreeCADGui - Win32 Debug" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADGui.mak" CFG="FreeCADGui - Win32 Debug" 
   cd "..\MOD\PART\GUI"

"FreeCADGui - Win32 DebugCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADGui.mak" CFG="FreeCADGui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\PART\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADDoc.mak" CFG="FreeCADApp - Win32 Release" 
   cd "..\MOD\PART\GUI"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADDoc.mak" CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\PART\GUI"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADDoc.mak" CFG="FreeCADApp - Win32 Debug" 
   cd "..\MOD\PART\GUI"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADDoc.mak" CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\PART\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADBase.mak" CFG="FreeCADBase - Win32 Release" 
   cd "..\MOD\PART\GUI"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADBase.mak" CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\PART\GUI"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADBase.mak" CFG="FreeCADBase - Win32 Debug" 
   cd "..\MOD\PART\GUI"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F ".\FreeCADBase.mak" CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\PART\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"AppPart - Win32 Release" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\AppPart.mak" CFG="AppPart - Win32 Release" 
   cd "..\GUI"

"AppPart - Win32 ReleaseCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\AppPart.mak" CFG="AppPart - Win32 Release" RECURSE=1 CLEAN 
   cd "..\GUI"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"AppPart - Win32 Debug" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\AppPart.mak" CFG="AppPart - Win32 Debug" 
   cd "..\GUI"

"AppPart - Win32 DebugCLEAN" : 
   cd "\Dokumente und Einstellungen\jriegel\Eigene Dateien\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F ".\AppPart.mak" CFG="AppPart - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\GUI"

!ENDIF 

SOURCE=.\AppPartGui.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AppPartGui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AppPartGui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Command.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPartBox.h" ".\DlgPartBoxImp.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPartBoxImp.h" ".\DlgPartBox.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\DlgPartBox.ui

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

InputDir=.
InputPath=.\DlgPartBox.ui
InputName=DlgPartBox

".\DlgPartBox.h"	".\DlgPartBox.cpp"	".\moc_DlgPartBox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

InputDir=.
InputPath=.\DlgPartBox.ui
InputName=DlgPartBox

".\DlgPartBox.h"	".\DlgPartBox.cpp"	".\moc_DlgPartBox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\DlgPartBoxImp.cpp

"$(INTDIR)\DlgPartBoxImp.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPartBox.h" ".\DlgPartBox.cpp" ".\moc_DlgPartBox.cpp" ".\moc_DlgPartBoxImp.cpp" ".\DlgPartBoxImp.h"


SOURCE=.\DlgPartBoxImp.h

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

InputDir=.
InputPath=.\DlgPartBoxImp.h
InputName=DlgPartBoxImp

".\moc_DlgPartBoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

InputDir=.
InputPath=.\DlgPartBoxImp.h
InputName=DlgPartBoxImp

".\moc_DlgPartBoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppPartGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppPartGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

