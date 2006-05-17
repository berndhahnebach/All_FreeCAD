# Microsoft Developer Studio Generated NMAKE File, Based on App_TEMPLATE_.dsp
!IF "$(CFG)" == ""
CFG=App_TEMPLATE_ - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. App_TEMPLATE_ - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "App_TEMPLATE_ - Win32 Release" && "$(CFG)" != "App_TEMPLATE_ - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "App_TEMPLATE_.mak" CFG="App_TEMPLATE_ - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "App_TEMPLATE_ - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "App_TEMPLATE_ - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\_TEMPLATE_.pyd" "$(OUTDIR)\App\Release\App_TEMPLATE_.pch"

!ELSE 

ALL : "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "$(OUTDIR)\_TEMPLATE_.pyd" "$(OUTDIR)\App\Release\App_TEMPLATE_.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\App_TEMPLATE_.obj"
	-@erase "$(INTDIR)\App_TEMPLATE_.pch"
	-@erase "$(INTDIR)\Feature_TEMPLATE_Box.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\_TEMPLATE_.exp"
	-@erase "$(OUTDIR)\_TEMPLATE_.lib"
	-@erase "$(OUTDIR)\_TEMPLATE_.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\App_TEMPLATE_.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\_TEMPLATE_.pdb" /machine:I386 /out:"$(OUTDIR)\_TEMPLATE_.pyd" /implib:"$(OUTDIR)\_TEMPLATE_.lib" 
LINK32_OBJS= \
	"$(INTDIR)\App_TEMPLATE_.obj" \
	"$(INTDIR)\Feature_TEMPLATE_Box.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib"

"$(OUTDIR)\_TEMPLATE_.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\_TEMPLATE__d.pyd" "$(OUTDIR)\App\Debug\App_TEMPLATE_.pch" "$(OUTDIR)\App_TEMPLATE_.bsc"

!ELSE 

ALL : "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "$(OUTDIR)\_TEMPLATE__d.pyd" "$(OUTDIR)\App\Debug\App_TEMPLATE_.pch" "$(OUTDIR)\App_TEMPLATE_.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\App_TEMPLATE_.obj"
	-@erase "$(INTDIR)\App_TEMPLATE_.pch"
	-@erase "$(INTDIR)\App_TEMPLATE_.sbr"
	-@erase "$(INTDIR)\Feature_TEMPLATE_Box.obj"
	-@erase "$(INTDIR)\Feature_TEMPLATE_Box.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\App_TEMPLATE_.bsc"
	-@erase "$(OUTDIR)\_TEMPLATE__d.exp"
	-@erase "$(OUTDIR)\_TEMPLATE__d.ilk"
	-@erase "$(OUTDIR)\_TEMPLATE__d.lib"
	-@erase "$(OUTDIR)\_TEMPLATE__d.pdb"
	-@erase "$(OUTDIR)\_TEMPLATE__d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\App_TEMPLATE_.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\App_TEMPLATE_.sbr" \
	"$(INTDIR)\Feature_TEMPLATE_Box.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\App_TEMPLATE_.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\_TEMPLATE__d.pdb" /debug /machine:I386 /out:"$(OUTDIR)\_TEMPLATE__d.pyd" /implib:"$(OUTDIR)\_TEMPLATE__d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\App_TEMPLATE_.obj" \
	"$(INTDIR)\Feature_TEMPLATE_Box.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib"

"$(OUTDIR)\_TEMPLATE__d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("App_TEMPLATE_.dep")
!INCLUDE "App_TEMPLATE_.dep"
!ELSE 
!MESSAGE Warning: cannot find "App_TEMPLATE_.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "App_TEMPLATE_ - Win32 Release" || "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\Mod\_TEMPLATE_\App"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\_TEMPLATE_\App"

!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\Mod\_TEMPLATE_\App"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\_TEMPLATE_\App"

!ENDIF 

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\Mod\_TEMPLATE_\App"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\_TEMPLATE_\App"

!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\Mod\_TEMPLATE_\App"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\_TEMPLATE_\App"

!ENDIF 

SOURCE=.\App_TEMPLATE_.cpp

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\App_TEMPLATE_.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\App_TEMPLATE_.obj"	"$(INTDIR)\App_TEMPLATE_.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Feature_TEMPLATE_Box.cpp

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Feature_TEMPLATE_Box.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Feature_TEMPLATE_Box.obj"	"$(INTDIR)\Feature_TEMPLATE_Box.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "App_TEMPLATE_ - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "FCApp_TEMPLATE_" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\App_TEMPLATE_.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "App_TEMPLATE_ - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_USRDLL" /D "FCApp_TEMPLATE_" /D "FC_DEBUG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\App_TEMPLATE_.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\App_TEMPLATE_.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

