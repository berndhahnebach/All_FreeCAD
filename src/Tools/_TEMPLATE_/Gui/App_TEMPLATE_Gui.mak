# Microsoft Developer Studio Generated NMAKE File, Based on App_TEMPLATE_Gui.dsp
!IF "$(CFG)" == ""
CFG=App_TEMPLATE_Gui - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. App_TEMPLATE_Gui - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "App_TEMPLATE_Gui - Win32 Release" && "$(CFG)" != "App_TEMPLATE_Gui - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "App_TEMPLATE_Gui.mak" CFG="App_TEMPLATE_Gui - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "App_TEMPLATE_Gui - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "App_TEMPLATE_Gui - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\_TEMPLATE_Gui.pyd" "$(OUTDIR)\App_TEMPLATE_Gui.pch"

!ELSE 

ALL : "App_TEMPLATE_ - Win32 Release" "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "FreeCADGui - Win32 Release" "..\_TEMPLATE_Gui.pyd" "$(OUTDIR)\App_TEMPLATE_Gui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 ReleaseCLEAN" "FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" "App_TEMPLATE_ - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\App_TEMPLATE_Gui.obj"
	-@erase "$(INTDIR)\App_TEMPLATE_Gui.pch"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Dlg_TEMPLATE_BoxImp.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\_TEMPLATE_Gui.exp"
	-@erase "$(OUTDIR)\_TEMPLATE_Gui.lib"
	-@erase "..\_TEMPLATE_Gui.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\App_TEMPLATE_Gui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\_TEMPLATE_Gui.pdb" /machine:I386 /out:"../_TEMPLATE_Gui.pyd" /implib:"$(OUTDIR)\_TEMPLATE_Gui.lib" 
LINK32_OBJS= \
	"$(INTDIR)\App_TEMPLATE_Gui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\Dlg_TEMPLATE_BoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Release\FreeCADGui.lib" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib" \
	"..\_TEMPLATE_.lib"

"..\_TEMPLATE_Gui.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\_TEMPLATE_Gui_d.pyd" "$(OUTDIR)\App_TEMPLATE_Gui.pch" "$(OUTDIR)\App_TEMPLATE_Gui.bsc"

!ELSE 

ALL : "App_TEMPLATE_ - Win32 Debug" "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "FreeCADGui - Win32 Debug" "..\_TEMPLATE_Gui_d.pyd" "$(OUTDIR)\App_TEMPLATE_Gui.pch" "$(OUTDIR)\App_TEMPLATE_Gui.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADGui - Win32 DebugCLEAN" "FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" "App_TEMPLATE_ - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\App_TEMPLATE_Gui.obj"
	-@erase "$(INTDIR)\App_TEMPLATE_Gui.pch"
	-@erase "$(INTDIR)\App_TEMPLATE_Gui.sbr"
	-@erase "$(INTDIR)\Command.obj"
	-@erase "$(INTDIR)\Command.sbr"
	-@erase "$(INTDIR)\Dlg_TEMPLATE_BoxImp.obj"
	-@erase "$(INTDIR)\Dlg_TEMPLATE_BoxImp.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\App_TEMPLATE_Gui.bsc"
	-@erase "$(OUTDIR)\_TEMPLATE_Gui_d.exp"
	-@erase "$(OUTDIR)\_TEMPLATE_Gui_d.lib"
	-@erase "$(OUTDIR)\_TEMPLATE_Gui_d.pdb"
	-@erase "..\_TEMPLATE_Gui_d.ilk"
	-@erase "..\_TEMPLATE_Gui_d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\App_TEMPLATE_Gui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\App_TEMPLATE_Gui.sbr" \
	"$(INTDIR)\Command.sbr" \
	"$(INTDIR)\Dlg_TEMPLATE_BoxImp.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\App_TEMPLATE_Gui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\_TEMPLATE_Gui_d.pdb" /debug /machine:I386 /out:"../_TEMPLATE_Gui_d.pyd" /implib:"$(OUTDIR)\_TEMPLATE_Gui_d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\App_TEMPLATE_Gui.obj" \
	"$(INTDIR)\Command.obj" \
	"$(INTDIR)\Dlg_TEMPLATE_BoxImp.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\Gui\Debug\FreeCADGuiD.lib" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib" \
	"..\_TEMPLATE__d.lib"

"..\_TEMPLATE_Gui_d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("App_TEMPLATE_Gui.dep")
!INCLUDE "App_TEMPLATE_Gui.dep"
!ELSE 
!MESSAGE Warning: cannot find "App_TEMPLATE_Gui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release" || "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

"FreeCADGui - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" 
   cd "..\MOD\_TEMPLATE_\GUI"

"FreeCADGui - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\_TEMPLATE_\GUI"

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

"FreeCADGui - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" 
   cd "..\MOD\_TEMPLATE_\GUI"

"FreeCADGui - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Gui"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADGui.mak CFG="FreeCADGui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\_TEMPLATE_\GUI"

!ENDIF 

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\MOD\_TEMPLATE_\GUI"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\_TEMPLATE_\GUI"

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\MOD\_TEMPLATE_\GUI"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\_TEMPLATE_\GUI"

!ENDIF 

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\MOD\_TEMPLATE_\GUI"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\MOD\_TEMPLATE_\GUI"

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\MOD\_TEMPLATE_\GUI"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\MOD\_TEMPLATE_\GUI"

!ENDIF 

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

"App_TEMPLATE_ - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\_TEMPLATE_\App"
   $(MAKE) /$(MAKEFLAGS) /F .\App_TEMPLATE_.mak CFG="App_TEMPLATE_ - Win32 Release" 
   cd "..\GUI"

"App_TEMPLATE_ - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\_TEMPLATE_\App"
   $(MAKE) /$(MAKEFLAGS) /F .\App_TEMPLATE_.mak CFG="App_TEMPLATE_ - Win32 Release" RECURSE=1 CLEAN 
   cd "..\GUI"

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

"App_TEMPLATE_ - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\_TEMPLATE_\App"
   $(MAKE) /$(MAKEFLAGS) /F .\App_TEMPLATE_.mak CFG="App_TEMPLATE_ - Win32 Debug" 
   cd "..\GUI"

"App_TEMPLATE_ - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Mod\_TEMPLATE_\App"
   $(MAKE) /$(MAKEFLAGS) /F .\App_TEMPLATE_.mak CFG="App_TEMPLATE_ - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\GUI"

!ENDIF 

SOURCE=.\App_TEMPLATE_Gui.cpp

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\App_TEMPLATE_Gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\App_TEMPLATE_Gui.obj"	"$(INTDIR)\App_TEMPLATE_Gui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Command.cpp

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Command.obj" : $(SOURCE) "$(INTDIR)" ".\Dlg_TEMPLATE_BoxImp.h" ".\Dlg_TEMPLATE_Box.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Command.obj"	"$(INTDIR)\Command.sbr" : $(SOURCE) "$(INTDIR)" ".\Dlg_TEMPLATE_BoxImp.h" ".\Dlg_TEMPLATE_Box.h"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Dlg_TEMPLATE_Box.ui

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

InputDir=.
InputPath=.\Dlg_TEMPLATE_Box.ui
InputName=Dlg_TEMPLATE_Box

".\Dlg_TEMPLATE_Box.h"	".\Dlg_TEMPLATE_Box.cpp"	".\moc_Dlg_TEMPLATE_Box.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

InputDir=.
InputPath=.\Dlg_TEMPLATE_Box.ui
InputName=Dlg_TEMPLATE_Box

".\Dlg_TEMPLATE_Box.h"	".\Dlg_TEMPLATE_Box.cpp"	".\moc_Dlg_TEMPLATE_Box.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%qtdir%\bin\uic.exe $(InputPath) -o $(InputDir)\$(InputName).h 
	%qtdir%\bin\uic.exe $(InputPath) -i $(InputName).h -o $(InputDir)\$(InputName).cpp 
	%qtdir%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\Dlg_TEMPLATE_BoxImp.cpp

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"


"$(INTDIR)\Dlg_TEMPLATE_BoxImp.obj" : $(SOURCE) "$(INTDIR)" ".\Dlg_TEMPLATE_BoxImp.h" ".\Dlg_TEMPLATE_Box.h" ".\Dlg_TEMPLATE_Box.cpp" ".\moc_Dlg_TEMPLATE_Box.cpp" ".\moc_Dlg_TEMPLATE_BoxImp.cpp"


!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"


"$(INTDIR)\Dlg_TEMPLATE_BoxImp.obj"	"$(INTDIR)\Dlg_TEMPLATE_BoxImp.sbr" : $(SOURCE) "$(INTDIR)" ".\Dlg_TEMPLATE_Box.cpp" ".\moc_Dlg_TEMPLATE_Box.cpp" ".\Dlg_TEMPLATE_BoxImp.h" ".\Dlg_TEMPLATE_Box.h" ".\moc_Dlg_TEMPLATE_BoxImp.cpp"


!ENDIF 

SOURCE=.\Dlg_TEMPLATE_BoxImp.h

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

InputDir=.
InputPath=.\Dlg_TEMPLATE_BoxImp.h
InputName=Dlg_TEMPLATE_BoxImp

".\moc_Dlg_TEMPLATE_BoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

InputDir=.
InputPath=.\Dlg_TEMPLATE_BoxImp.h
InputName=Dlg_TEMPLATE_BoxImp

".\moc_Dlg_TEMPLATE_BoxImp.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	%QTDIR%\bin\moc.exe $(InputDir)\$(InputName).h -o $(InputDir)\moc_$(InputName).cpp
<< 
	

!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\App_TEMPLATE_Gui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_Gui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_Gui" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_Gui.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\App_TEMPLATE_Gui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

