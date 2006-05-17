# Microsoft Developer Studio Generated NMAKE File, Based on AppPointsGui.dsp
!IF "$(CFG)" == ""
CFG=AppPointsGui - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. AppPointsGui - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "AppPointsGui - Win32 Release" && "$(CFG)" != "AppPointsGui - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppPointsGui.mak" CFG="AppPointsGui - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppPointsGui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppPointsGui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\PointsGui.pyd" "$(OUTDIR)\AppPointsGui.pch"

!ELSE 

ALL : "AppPoints - Win32 Release" "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "FreeCADGui - Win32 Release" "..\PointsGui.pyd" "$(OUTDIR)\AppPointsGui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 ReleaseCLEAN" "FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" "AppPoints - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPointsGui.obj"
	-@erase "$(INTDIR)\AppPointsGui.pch"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\DlgPointsBoxImp.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PointsGui.exp"
	-@erase "$(OUTDIR)\PointsGui.lib"
	-@erase "..\PointsGui.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPointsGui" /Fp"$(INTDIR)\AppPointsGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPointsGui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\PointsGui.pdb" /machine:I386 /out:"../PointsGui.pyd" /implib:"$(OUTDIR)\PointsGui.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AppPointsGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgPointsBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Release\FreeCADGui.lib" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib" \
	"..\Points.lib"

"..\PointsGui.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\PointsGui_d.pyd" "$(OUTDIR)\AppPointsGui.pch" "$(OUTDIR)\AppPointsGui.bsc"

!ELSE 

ALL : "AppPoints - Win32 Debug" "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "FreeCADGui - Win32 Debug" "..\PointsGui_d.pyd" "$(OUTDIR)\AppPointsGui.pch" "$(OUTDIR)\AppPointsGui.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 DebugCLEAN" "FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" "AppPoints - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPointsGui.obj"
	-@erase "$(INTDIR)\AppPointsGui.pch"
	-@erase "$(INTDIR)\AppPointsGui.sbr"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Command.sbr"
	-@erase "$(INTDIR)\DlgPointsBoxImp.obj"
	-@erase "$(INTDIR)\DlgPointsBoxImp.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AppPointsGui.bsc"
	-@erase "$(OUTDIR)\PointsGui_d.exp"
	-@erase "$(OUTDIR)\PointsGui_d.lib"
	-@erase "$(OUTDIR)\PointsGui_d.pdb"
	-@erase "..\PointsGui_d.ilk"
	-@erase "..\PointsGui_d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPointsGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPointsGui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPointsGui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AppPointsGui.sbr" \
	"$(INTDIR)\Command.sbr" \
	"$(INTDIR)\DlgPointsBoxImp.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\AppPointsGui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\PointsGui_d.pdb" /debug /machine:I386 /out:"../PointsGui_d.pyd" /implib:"$(OUTDIR)\PointsGui_d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AppPointsGui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\DlgPointsBoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Debug\FreeCADGuiD.lib" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib" \
	"..\Points_d.lib"

"..\PointsGui_d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AppPointsGui.dep")
!INCLUDE "AppPointsGui.dep"
!ELSE 
!MESSAGE Warning: cannot find "AppPointsGui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AppPointsGui - Win32 Release" || "$(CFG)" == "AppPointsGui - Win32 Debug"

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

"FreeCADGui - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" 
   cd "..\MOD\Points\GUI"

"FreeCADGui - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\Points\GUI"

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

"FreeCADGui - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" 
   cd "..\MOD\Points\GUI"

"FreeCADGui - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\Points\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\MOD\Points\GUI"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\Points\GUI"

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\MOD\Points\GUI"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\Points\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\MOD\Points\GUI"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\Points\GUI"

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\MOD\Points\GUI"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\Points\GUI"

!ENDIF 

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

"AppPoints - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Points\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPoints.mak CFG="AppPoints - Win32 Release" 
   cd "..\GUI"

"AppPoints - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Points\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPoints.mak CFG="AppPoints - Win32 Release" RECURSE=1 CLEAN 
   cd "..\GUI"

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

"AppPoints - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Points\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPoints.mak CFG="AppPoints - Win32 Debug" 
   cd "..\GUI"

"AppPoints - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\Points\App"
   $(MAKE) /$(MAKEFLAGS) /F .\AppPoints.mak CFG="AppPoints - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\GUI"

!ENDIF 

SOURCE=.\AppPointsGui.cpp

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPointsGui" /Fp"$(INTDIR)\AppPointsGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AppPointsGui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPointsGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPointsGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AppPointsGui.obj"	"$(INTDIR)\AppPointsGui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Command.cpp

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPointsGui" /Fp"$(INTDIR)\AppPointsGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPointsBoxImp.h" ".\DlgPointsBox.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPointsGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPointsGui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Command.obj"	"$(INTDIR)\Command.sbr" : $(SOURCE) "$(INTDIR)" ".\DlgPointsBoxImp.h" ".\DlgPointsBox.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\DlgPointsBox.ui

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

InputDir=.
InputPath=.\DlgPointsBox.ui
InputName=DlgPointsBox

".\DlgPointsBox.h"	".\DlgPointsBox.cpp"	".\moc_DlgPointsBox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

InputDir=.
InputPath=.\DlgPointsBox.ui
InputName=DlgPointsBox

".\DlgPointsBox.h"	".\DlgPointsBox.cpp"	".\moc_DlgPointsBox.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\DlgPointsBoxImp.cpp

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"


"$(INTDIR)\DlgPointsBoxImp.obj" : $(SOURCE) "$(INTDIR)" ".\DlgPointsBoxImp.h" ".\DlgPointsBox.h" ".\DlgPointsBox.cpp" ".\moc_DlgPointsBox.cpp" ".\moc_DlgPointsBoxImp.cpp"


!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"


"$(INTDIR)\DlgPointsBoxImp.obj"	"$(INTDIR)\DlgPointsBoxImp.sbr" : $(SOURCE) "$(INTDIR)" ".\DlgPointsBox.cpp" ".\moc_DlgPointsBox.cpp" ".\DlgPointsBoxImp.h" ".\DlgPointsBox.h" ".\moc_DlgPointsBoxImp.cpp"


!ENDIF 

SOURCE=.\DlgPointsBoxImp.h

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

InputDir=.
InputPath=.\DlgPointsBoxImp.h
InputName=DlgPointsBoxImp

".\moc_DlgPointsBoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

InputDir=.
InputPath=.\DlgPointsBoxImp.h
InputName=DlgPointsBoxImp

".\moc_DlgPointsBoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPointsGui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPointsGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "AppPointsGui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCAppPointsGui" /Fp"$(INTDIR)\AppPointsGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppPointsGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPointsGui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCAppPointsGui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPointsGui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\AppPointsGui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

