# Microsoft Developer Studio Project File - Name="FreeCADMain" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=FreeCADMain - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADMain.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FreeCADMain.mak" CFG="FreeCADMain - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FreeCADMain - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "FreeCADMain - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FreeCADMain - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "NDEBUG" /D "_CONSOLE" /D "CSFDB" /D "WIN32" /D "_MBCS" /D "WNT" /D "__STLP_USE_DECLSPEC" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ALTERNATE_QTSMANIP" /D "NO_KDE2" /D "_STLP_NO_NEW_IOSTREAMS" /D "_STLP_USE_ABBREVS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Release/FreeCAD.exe"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\FreeCAD.exe ..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FreeCADMain - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "FreeCADMain___Win32_Debug"
# PROP BASE Intermediate_Dir "FreeCADMain___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "__STL_DEBUG" /D "_CONSOLE" /D "CSFDB" /D "WIN32" /D "_MBCS" /D "WNT" /D "__STLP_USE_DECLSPEC" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ALTERNATE_QTSMANIP" /D "NO_KDE2" /D "_STLP_NO_NEW_IOSTREAMS" /D "_STLP_USE_ABBREVS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/FreeCADD.exe" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\FreeCADD.exe ..\..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FreeCADMain - Win32 Release"
# Name "FreeCADMain - Win32 Debug"
# Begin Source File

SOURCE=.\FreeCADInit.py

!IF  "$(CFG)" == "FreeCADMain - Win32 Release"

# Begin Custom Build - Building InitScript.h
InputPath=.\FreeCADInit.py

"InitScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADInit.py InitScript.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADMain - Win32 Debug"

# Begin Custom Build - Building InitScript.h
InputPath=.\FreeCADInit.py

"InitScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADInit.py InitScript.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeCADInstall.py

!IF  "$(CFG)" == "FreeCADMain - Win32 Release"

# Begin Custom Build - Building InstallScript.h
InputPath=.\FreeCADInstall.py

"InstallScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADInstall.py InstallScript.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADMain - Win32 Debug"

# Begin Custom Build - Building InstallScript.h
InputPath=.\FreeCADInstall.py

"InstallScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADInstall.py InstallScript.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeCADTest.py

!IF  "$(CFG)" == "FreeCADMain - Win32 Release"

# Begin Custom Build - Building TestScript.h
InputPath=.\FreeCADTest.py

"TestScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADTest.py TestScript.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADMain - Win32 Debug"

# Begin Custom Build - Building TestScript.h
InputPath=.\FreeCADTest.py

"TestScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADTest.py TestScript.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeCADTestEnv.py

!IF  "$(CFG)" == "FreeCADMain - Win32 Release"

# Begin Custom Build - Building TestEnvScript.h
InputPath=.\FreeCADTestEnv.py

"TestEnvScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADTestEnv.py TestEnvScript.h

# End Custom Build

!ELSEIF  "$(CFG)" == "FreeCADMain - Win32 Debug"

# Begin Custom Build - Building TestEnvScript.h
InputPath=.\FreeCADTestEnv.py

"TestEnvScript.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	python ..\Tools\PythonToCPP.py FreeCADTestEnv.py TestEnvScript.h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Libs.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# End Target
# End Project
