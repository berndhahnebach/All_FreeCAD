# Microsoft Developer Studio Project File - Name="AppTestGui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=AppTestGui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AppTestGui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AppTestGui.mak" CFG="AppTestGui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AppTestGui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AppTestGui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPQtUnitGUI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../../../" /D "NDEBUG" /D "_USRDLL" /D "FCAppQtUnitGui" /Yu"PreCompiled.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 MSVCRT.LIB MSVCPRT.LIB kernel32.lib python23.lib coin2.lib qt-mtnc321.lib /nologo /dll /machine:I386 /nodefaultlib /out:"Release\QtUnitGui.pyd"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy    /y    /f    Release\QtUnitGui.pyd    ..\..\..\..\Mod\Test\   	xcopy    /y    /f    qtunittest.py    ..\..\..\..\Mod\Test\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "APPQtUnitGUI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../../" /D "_USRDLL" /D "FCAppQtUnitGui" /D "FC_DEBUG" /FR /Yu"PreCompiled.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 MSVCRTD.LIB MSVCPRTD.LIB kernel32.lib python23_d.lib coin2d.lib qt-mtnc321.lib /nologo /dll /debug /machine:I386 /nodefaultlib /out:"Debug\QtUnitGui_d.pyd" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=xcopy    /y    /f    Debug\QtUnitGui_d.pyd    ..\..\..\..\Mod\Test\   	xcopy    /y    /f    qtunittest.py    ..\..\..\..\Mod\Test\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AppTestGui - Win32 Release"
# Name "AppTestGui - Win32 Debug"
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\qtunittest.py
# End Source File
# Begin Source File

SOURCE=.\UnitTest.ui

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\UnitTest.ui
InputName=UnitTest

BuildCmds= \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

# Begin Custom Build - Uic'ing $(InputName).ui ...
InputDir=.
InputPath=.\UnitTest.ui
InputName=UnitTest

BuildCmds= \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -o "$(InputDir)\$(InputName).h" \
	"%qtdir%\bin\uic.exe" "$(InputPath)" -i "$(InputName).h" -o "$(InputDir)\$(InputName).cpp" \
	"%qtdir%\bin\moc.exe" "$(InputDir)\$(InputName).h" -o "$(InputDir)\moc_$(InputName).cpp" \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UnitTestImp.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitTestImp.h
# End Source File
# Begin Source File

SOURCE=.\UnitTestPy.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitTestPy.h
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADApp.lib

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADAppD.lib

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADBase.lib

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADBaseD.lib

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADGui.lib

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\lib\FreeCADGuiD.lib

!IF  "$(CFG)" == "AppTestGui - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "AppTestGui - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\AppTestGui.cpp
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.cpp
# ADD CPP /Yc"PreCompiled.h"
# End Source File
# Begin Source File

SOURCE=.\PreCompiled.h
# End Source File
# End Target
# End Project
