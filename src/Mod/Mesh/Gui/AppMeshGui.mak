# Microsoft Developer Studio Generated NMAKE File, Based on AppMeshGui.dsp
!IF "$(CFG)" == ""
CFG=AppMeshGui - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. AppMeshGui - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "AppMeshGui - Win32 Release" && "$(CFG)" != "AppMeshGui - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppMeshGui.mak" CFG="AppMeshGui - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppMeshGui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppMeshGui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\MeshGui.pyd" "$(OUTDIR)\AppMeshGui.pch"

!ELSE 

ALL : "AppMesh - Win32 Release" "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "FreeCADGui - Win32 Release" "..\MeshGui.pyd" "$(OUTDIR)\AppMeshGui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 ReleaseCLEAN" "FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" "AppMesh - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppMeshGui.obj"
	-@erase "$(INTDIR)\AppMeshGui.pch"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\DlgMeshBoxImp.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\MeshGui.exp"
	-@erase "$(OUTDIR)\MeshGui.lib"
	-@erase "..\MeshGui.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppMeshGui" /Fp"$(INTDIR)\AppMeshGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppMeshGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\MeshGui.pdb" /machine:I386 /out:"../MeshGui.pyd" /implib:"$(OUTDIR)\MeshGui.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AppMeshGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgMeshBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Release\FreeCADGui.lib" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib" \
	"..\Mesh.lib"

"..\MeshGui.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\MeshGui_d.pyd" "$(OUTDIR)\AppMeshGui.pch" "$(OUTDIR)\AppMeshGui.bsc"

!ELSE 

ALL : "AppMesh - Win32 Debug" "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "FreeCADGui - Win32 Debug" "..\MeshGui_d.pyd" "$(OUTDIR)\AppMeshGui.pch" "$(OUTDIR)\AppMeshGui.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 DebugCLEAN" "FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" "AppMesh - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppMeshGui.obj"
	-@erase "$(INTDIR)\AppMeshGui.pch"
	-@erase "$(INTDIR)\AppMeshGui.sbr"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Command.sbr"
	-@erase "$(INTDIR)\DlgMeshBoxImp.obj"
	-@erase "$(INTDIR)\DlgMeshBoxImp.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AppMeshGui.bsc"
	-@erase "$(OUTDIR)\MeshGui_d.exp"
	-@erase "$(OUTDIR)\MeshGui_d.lib"
	-@erase "$(OUTDIR)\MeshGui_d.pdb"
	-@erase "..\MeshGui_d.ilk"
	-@erase "..\MeshGui_d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppMeshGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppMeshGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppMeshGui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AppMeshGui.sbr" \
	"$(INTDIR)\Command.sbr" \
	"$(INTDIR)\DlgMeshBoxImp.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\AppMeshGui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MeshGui_d.pdb" /debug /machine:I386 /out:"../MeshGui_d.pyd" /implib:"$(OUTDIR)\MeshGui_d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AppMeshGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgMeshBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Debug\FreeCADGuiD.lib" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib" \
	"..\Mesh_d.lib"

"..\MeshGui_d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AppMeshGui.dep")
!INCLUDE "AppMeshGui.dep"
!ELSE 
!MESSAGE Warning: cannot find "AppMeshGui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AppMeshGui - Win32 Release" || "$(CFG)" == "AppMeshGui - Win32 Debug"

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

"FreeCADGui - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" 
   cd "..\MOD\Mesh\GUI"

"FreeCADGui - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\Mesh\GUI"

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

"FreeCADGui - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" 
   cd "..\MOD\Mesh\GUI"

"FreeCADGui - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\Mesh\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\MOD\Mesh\GUI"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\Mesh\GUI"

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\MOD\Mesh\GUI"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\Mesh\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\MOD\Mesh\GUI"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\Mesh\GUI"

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\MOD\Mesh\GUI"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\Mesh\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

"AppMesh - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Mesh\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppMesh.mak CFG="AppMesh - Win32 Release" 
   cd "..\GUI"

"AppMesh - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Mesh\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppMesh.mak CFG="AppMesh - Win32 Release" RECURSE=1 CLEAN 
   cd "..\GUI"

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

"AppMesh - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Mesh\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppMesh.mak CFG="AppMesh - Win32 Debug" 
   cd "..\GUI"

"AppMesh - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Mesh\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppMesh.mak CFG="AppMesh - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\GUI"

!ENDIF 

SOURCE=.\AppMeshGui.cpp

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppMeshGui" /Fp"$(INTDIR)\AppMeshGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AppMeshGui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppMeshGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppMeshGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AppMeshGui.obj"	"$(INTDIR)\AppMeshGui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Command.cpp

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppMeshGui" /Fp"$(INTDIR)\AppMeshGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)" ".\DlgMeshBoxImp.h" ".\DlgMeshBox.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppMeshGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppMeshGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Command.obj"	"$(INTDIR)\Command.sbr" : $(SOURCE) "$(INTDIR)" ".\DlgMeshBoxImp.h" ".\DlgMeshBox.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\DlgMeshBox.ui

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

InputDir=.
InputPath=.\DlgMeshBox.ui
InputName=DlgMeshBox

".\DlgMeshBox.h"	".\DlgMeshBox.cpp"	".\moc_DlgMeshBox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

InputDir=.
InputPath=.\DlgMeshBox.ui
InputName=DlgMeshBox

".\DlgMeshBox.h"	".\DlgMeshBox.cpp"	".\moc_DlgMeshBox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\DlgMeshBoxImp.cpp

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"


"$(INTDIR)\DlgMeshBoxImp.obj" : $(SOURCE) "$(INTDIR)" ".\DlgMeshBoxImp.h" ".\DlgMeshBox.h" ".\DlgMeshBox.cpp" ".\moc_DlgMeshBox.cpp" ".\moc_DlgMeshBoxImp.cpp"


!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"


"$(INTDIR)\DlgMeshBoxImp.obj"	"$(INTDIR)\DlgMeshBoxImp.sbr" : $(SOURCE) "$(INTDIR)" ".\DlgMeshBox.cpp" ".\moc_DlgMeshBox.cpp" ".\DlgMeshBoxImp.h" ".\DlgMeshBox.h" ".\moc_DlgMeshBoxImp.cpp"


!ENDIF 

SOURCE=.\DlgMeshBoxImp.h

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

InputDir=.
InputPath=.\DlgMeshBoxImp.h
InputName=DlgMeshBoxImp

".\moc_DlgMeshBoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

InputDir=.
InputPath=.\DlgMeshBoxImp.h
InputName=DlgMeshBoxImp

".\moc_DlgMeshBoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppMeshGui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppMeshGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "AppMeshGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppMeshGui" /Fp"$(INTDIR)\AppMeshGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppMeshGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppMeshGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppMeshGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppMeshGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\AppMeshGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

