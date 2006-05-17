# Microsoft Developer Studio Generated NMAKE File, Based on AppImportGui.dsp
!IF "$(CFG)" == ""
CFG=AppImportGui - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. AppImportGui - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "AppImportGui - Win32 Release" && "$(CFG)" != "AppImportGui - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppImportGui.mak" CFG="AppImportGui - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppImportGui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppImportGui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\ImportGui.pyd" "$(OUTDIR)\AppImportGui.pch"

!ELSE 

ALL : "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "FreeCADGui - Win32 Release" "AppImport - Win32 Release" "..\ImportGui.pyd" "$(OUTDIR)\AppImportGui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"AppImport - Win32 ReleaseCLEAN" "FreeCADGui - Win32 ReleaseCLEAN" "FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppImportGui.obj"
	-@erase "$(INTDIR)\AppImportGui.pch"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ImportGui.exp"
	-@erase "$(OUTDIR)\ImportGui.lib"
	-@erase "..\ImportGui.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppImportGui" /Fp"$(INTDIR)\AppImportGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppImportGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ImportGui.pdb" /machine:I386 /out:"../ImportGui.pyd" /implib:"$(OUTDIR)\ImportGui.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AppImportGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\Import.lib" \
	"..\..\..\Gui\Release\FreeCADGui.lib" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib"

"..\ImportGui.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\ImportGui_d.pyd" "$(OUTDIR)\AppImportGui.pch" "$(OUTDIR)\AppImportGui.bsc"

!ELSE 

ALL : "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "FreeCADGui - Win32 Debug" "AppImport - Win32 Debug" "..\ImportGui_d.pyd" "$(OUTDIR)\AppImportGui.pch" "$(OUTDIR)\AppImportGui.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"AppImport - Win32 DebugCLEAN" "FreeCADGui - Win32 DebugCLEAN" "FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppImportGui.obj"
	-@erase "$(INTDIR)\AppImportGui.pch"
	-@erase "$(INTDIR)\AppImportGui.sbr"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Command.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AppImportGui.bsc"
	-@erase "$(OUTDIR)\ImportGui_d.exp"
	-@erase "$(OUTDIR)\ImportGui_d.lib"
	-@erase "$(OUTDIR)\ImportGui_d.pdb"
	-@erase "..\ImportGui_d.ilk"
	-@erase "..\ImportGui_d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppImportGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppImportGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppImportGui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AppImportGui.sbr" \
	"$(INTDIR)\Command.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\AppImportGui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ImportGui_d.pdb" /debug /machine:I386 /out:"../ImportGui_d.pyd" /implib:"$(OUTDIR)\ImportGui_d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AppImportGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\Import_d.lib" \
	"..\..\..\Gui\Debug\FreeCADGuiD.lib" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib"

"..\ImportGui_d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AppImportGui.dep")
!INCLUDE "AppImportGui.dep"
!ELSE 
!MESSAGE Warning: cannot find "AppImportGui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AppImportGui - Win32 Release" || "$(CFG)" == "AppImportGui - Win32 Debug"

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

"AppImport - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Import\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppImport.mak CFG="AppImport - Win32 Release" 
   cd "..\Gui"

"AppImport - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Import\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppImport.mak CFG="AppImport - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Gui"

!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

"AppImport - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Import\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppImport.mak CFG="AppImport - Win32 Debug" 
   cd "..\Gui"

"AppImport - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Import\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppImport.mak CFG="AppImport - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Gui"

!ENDIF 

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

"FreeCADGui - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" 
   cd "..\Mod\Import\Gui"

"FreeCADGui - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Import\Gui"

!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

"FreeCADGui - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" 
   cd "..\Mod\Import\Gui"

"FreeCADGui - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Import\Gui"

!ENDIF 

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\Mod\Import\Gui"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Import\Gui"

!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\Mod\Import\Gui"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Import\Gui"

!ENDIF 

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\Mod\Import\Gui"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Import\Gui"

!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\Mod\Import\Gui"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Import\Gui"

!ENDIF 

SOURCE=.\AppImportGui.cpp

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppImportGui" /Fp"$(INTDIR)\AppImportGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AppImportGui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppImportGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppImportGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AppImportGui.obj"	"$(INTDIR)\AppImportGui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Command.cpp

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppImportGui" /Fp"$(INTDIR)\AppImportGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppImportGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppImportGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Command.obj"	"$(INTDIR)\Command.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppImportGui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppImportGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "AppImportGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppImportGui" /Fp"$(INTDIR)\AppImportGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppImportGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppImportGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppImportGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppImportGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\AppImportGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

