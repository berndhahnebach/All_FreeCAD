# Microsoft Developer Studio Generated NMAKE File, Based on AppPart.dsp
!IF "$(CFG)" == ""
CFG=AppPart - Win32 Debug
!MESSAGE Keine Konfiguration angegeben. AppPart - Win32 Debug wird als Standard verwendet.
!ENDIF 

!IF "$(CFG)" != "AppPart - Win32 Release" && "$(CFG)" != "AppPart - Win32 Debug"
!MESSAGE UngÅltige Konfiguration "$(CFG)" angegeben.
!MESSAGE Sie kînnen beim AusfÅhren von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "AppPart.mak" CFG="AppPart - Win32 Debug"
!MESSAGE 
!MESSAGE FÅr die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "AppPart - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppPart - Win32 Debug" (basierend auf  "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AppPart - Win32 Release"

OUTDIR=.\..
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AppPart.pyd" "$(OUTDIR)\App\Release\AppPart.pch"

!ELSE 

ALL : "FreeCADBase - Win32 Release" "FreeCADApp - Win32 Release" "$(OUTDIR)\AppPart.pyd" "$(OUTDIR)\App\Release\AppPart.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADApp - Win32 ReleaseCLEAN" "FreeCADBase - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPart.obj"
	-@erase "$(INTDIR)\AppPart.pch"
	-@erase "$(INTDIR)\FeaturePartBox.obj"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AppPart.exp"
	-@erase "$(OUTDIR)\AppPart.lib"
	-@erase "$(OUTDIR)\AppPart.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /Fp"$(INTDIR)\AppPart.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPart.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\AppPart.pdb" /machine:I386 /out:"$(OUTDIR)\AppPart.pyd" /implib:"$(OUTDIR)\AppPart.lib" 
LINK32_OBJS= \
	"$(INTDIR)\AppPart.obj" \
	"$(INTDIR)\FeaturePartBox.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\App\Release\FreeCADApp.lib" \
	"..\..\..\Base\Release\FreeCADBase.lib"

"$(OUTDIR)\AppPart.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

OUTDIR=.\..
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\AppPart_d.pyd" "$(OUTDIR)\App\Debug\AppPart.pch" "$(OUTDIR)\AppPart.bsc"

!ELSE 

ALL : "FreeCADBase - Win32 Debug" "FreeCADApp - Win32 Debug" "$(OUTDIR)\AppPart_d.pyd" "$(OUTDIR)\App\Debug\AppPart.pch" "$(OUTDIR)\AppPart.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"FreeCADApp - Win32 DebugCLEAN" "FreeCADBase - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AppPart.obj"
	-@erase "$(INTDIR)\AppPart.pch"
	-@erase "$(INTDIR)\AppPart.sbr"
	-@erase "$(INTDIR)\FeaturePartBox.obj"
	-@erase "$(INTDIR)\FeaturePartBox.sbr"
	-@erase "$(INTDIR)\Libs.obj"
	-@erase "$(INTDIR)\Libs.sbr"
	-@erase "$(INTDIR)\PreCompiled.obj"
	-@erase "$(INTDIR)\PreCompiled.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AppPart.bsc"
	-@erase "$(OUTDIR)\AppPart_d.exp"
	-@erase "$(OUTDIR)\AppPart_d.ilk"
	-@erase "$(OUTDIR)\AppPart_d.lib"
	-@erase "$(OUTDIR)\AppPart_d.pdb"
	-@erase "$(OUTDIR)\AppPart_d.pyd"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPart.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AppPart.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\AppPart.sbr" \
	"$(INTDIR)\FeaturePartBox.sbr" \
	"$(INTDIR)\Libs.sbr" \
	"$(INTDIR)\PreCompiled.sbr"

"$(OUTDIR)\AppPart.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\AppPart_d.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AppPart_d.pyd" /implib:"$(OUTDIR)\AppPart_d.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AppPart.obj" \
	"$(INTDIR)\FeaturePartBox.obj" \
	"$(INTDIR)\Libs.obj" \
	"$(INTDIR)\PreCompiled.obj" \
	"..\..\..\App\Debug\FreeCADAppD.lib" \
	"..\..\..\Base\Debug\FreeCADBaseD.lib"

"$(OUTDIR)\AppPart_d.pyd" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AppPart.dep")
!INCLUDE "AppPart.dep"
!ELSE 
!MESSAGE Warning: cannot find "AppPart.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AppPart - Win32 Release" || "$(CFG)" == "AppPart - Win32 Debug"

!IF  "$(CFG)" == "AppPart - Win32 Release"

"FreeCADApp - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" 
   cd "..\Mod\Part\App"

"FreeCADApp - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Part\App"

!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

"FreeCADApp - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" 
   cd "..\Mod\Part\App"

"FreeCADApp - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\App"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Part\App"

!ENDIF 

!IF  "$(CFG)" == "AppPart - Win32 Release"

"FreeCADBase - Win32 Release" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" 
   cd "..\Mod\Part\App"

"FreeCADBase - Win32 ReleaseCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\Mod\Part\App"

!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

"FreeCADBase - Win32 Debug" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" 
   cd "..\Mod\Part\App"

"FreeCADBase - Win32 DebugCLEAN" : 
   cd "\Develop\Projekte\FreeCAD_0.1\src\Base"
   $(MAKE) /$(MAKEFLAGS) /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\Mod\Part\App"

!ENDIF 

SOURCE=.\AppPart.cpp

!IF  "$(CFG)" == "AppPart - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /Fp"$(INTDIR)\AppPart.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AppPart.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPart.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AppPart.obj"	"$(INTDIR)\AppPart.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\FeaturePartBox.cpp

!IF  "$(CFG)" == "AppPart - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /Fp"$(INTDIR)\AppPart.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\FeaturePartBox.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPart.pch" /YX"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\FeaturePartBox.obj"	"$(INTDIR)\FeaturePartBox.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\Libs.cpp

!IF  "$(CFG)" == "AppPart - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Libs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\Libs.obj"	"$(INTDIR)\Libs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\PreCompiled.cpp

!IF  "$(CFG)" == "AppPart - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /Fp"$(INTDIR)\AppPart.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\AppPart.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "AppPart - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "_MBCS" /D "WNT" /D "FCAppPart" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\AppPart.pch" /Yc"PreCompiled.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\PreCompiled.obj"	"$(INTDIR)\PreCompiled.sbr"	"$(INTDIR)\AppPart.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

