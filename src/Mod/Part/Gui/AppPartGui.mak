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

ALL : "..\PartGui.pyd" "$(OUTDIR)\AppPartGui.pch"

!ELSE 

ALL : "AppPart - Win32 Release" "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "FreeCADGui - Win32 Release" "..\PartGui.pyd" "$(OUTDIR)\AppPartGui.pch"

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
	-@erase "$(OUTDIR)\PartGui.exp"
	-@erase "$(OUTDIR)\PartGui.lib"
	-@erase "..\PartGui.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPartGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\PartGui.pdb" /machine:I386 /out:"../PartGui.pyd" /implib:"$(OUTDIR)\PartGui.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AppPartGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgPartBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Release\FreeCADGui.lib" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib" \
	"..\Part.lib"

"..\PartGui.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

ALL : "..\PartGui_d.pyd" "$(OUTDIR)\AppPartGui.pch" "$(OUTDIR)\AppPartGui.bsc"

!ELSE 

ALL : "AppPart - Win32 Debug" "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "FreeCADGui - Win32 Debug" "..\PartGui_d.pyd" "$(OUTDIR)\AppPartGui.pch" "$(OUTDIR)\AppPartGui.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 DebugCLEAN" "FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" "AppPart - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPartGui.obj"
	-@erase "$(INTDIR)\AppPartGui.pch"
	-@erase "$(INTDIR)\AppPartGui.sbr"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Command.sbr"
	-@erase "$(INTDIR)\DlgPartBoxImp.obj"
	-@erase "$(INTDIR)\DlgPartBoxImp.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AppPartGui.bsc"
	-@erase "$(OUTDIR)\PartGui_d.exp"
	-@erase "$(OUTDIR)\PartGui_d.lib"
	-@erase "$(OUTDIR)\PartGui_d.pdb"
	-@erase "..\PartGui_d.ilk"
	-@erase "..\PartGui_d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPartGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPartGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPartGui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AppPartGui.sbr" \
	"$(INTDIR)\Command.sbr" \
	"$(INTDIR)\DlgPartBoxImp.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\AppPartGui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\PartGui_d.pdb" /debug /machine:I386 /out:"../PartGui_d.pyd" /implib:"$(OUTDIR)\PartGui_d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AppPartGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgPartBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Debug\FreeCADGuiD.lib" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib" \
	"..\Part_d.lib"

"..\PartGui_d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" 
   cd "..\Mod\Part\Gui"

"FreeCADGui - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Part\Gui"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"FreeCADGui - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" 
   cd "..\Mod\Part\Gui"

"FreeCADGui - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Part\Gui"

!ENDIF 

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\Mod\Part\Gui"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Part\Gui"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\Mod\Part\Gui"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Part\Gui"

!ENDIF 

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\Mod\Part\Gui"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Part\Gui"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\Mod\Part\Gui"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Part\Gui"

!ENDIF 

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

"AppPart - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPart.mak CFG="AppPart - Win32 Release" 
   cd "..\Gui"

"AppPart - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPart.mak CFG="AppPart - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Gui"

!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

"AppPart - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPart.mak CFG="AppPart - Win32 Debug" 
   cd "..\Gui"

"AppPart - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Part\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPart.mak CFG="AppPart - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Gui"

!ENDIF 

SOURCE=.\AppPartGui.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AppPartGui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPartGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AppPartGui.obj"	"$(INTDIR)\AppPartGui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Command.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPartBoxImp.h" ".\DlgPartBox.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPartGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPartGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Command.obj"	"$(INTDIR)\Command.sbr" : $(SOURCE) "$(INTDIR)" ".\DlgPartBoxImp.h" ".\DlgPartBox.h"
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

!IF  "$(CFG)" == "AppPartGui - Win32 Release"


"$(INTDIR)\DlgPartBoxImp.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPartBoxImp.h" ".\DlgPartBox.h" ".\DlgPartBox.cpp" ".\moc_DlgPartBox.cpp" ".\moc_DlgPartBoxImp.cpp"


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"


"$(INTDIR)\DlgPartBoxImp.obj"	"$(INTDIR)\DlgPartBoxImp.sbr" : $(SOURCE) "$(INTDIR)" ".\DlgPartBox.cpp" ".\moc_DlgPartBox.cpp" ".\DlgPartBoxImp.h" ".\DlgPartBox.h" ".\moc_DlgPartBoxImp.cpp"


!ENDIF 

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

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPartGui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPartGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "AppPartGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPartGui" /Fp"$(INTDIR)\AppPartGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppPartGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPartGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPartGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPartGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\AppPartGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

