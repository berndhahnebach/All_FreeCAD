cd Base
nmake /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release" CLEAN
nmake /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Release"
nmake /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug" CLEAN
nmake /F .\FreeCADBase.mak CFG="FreeCADBase - Win32 Debug"
cd ..\App
nmake /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release" CLEAN
nmake /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Release"
nmake /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug" CLEAN
nmake /F .\FreeCADDoc.mak CFG="FreeCADApp - Win32 Debug"
cd ..\Main
nmake /F .\FreeCADMain.mak CFG="FreeCADMain - Win32 Release" CLEAN
nmake /F .\FreeCADMain.mak CFG="FreeCADMain - Win32 Release"
nmake /F .\FreeCADMainCmd.mak CFG="FreeCADMainCmd - Win32 Release" CLEAN
nmake /F .\FreeCADMainCmd.mak CFG="FreeCADMainCmd - Win32 Release"
nmake /F .\FreeCADMainPy.mak CFG="FreeCADMainPy - Win32 Release" CLEAN
nmake /F .\FreeCADMainPy.mak CFG="FreeCADMainPy - Win32 Release"
nmake /F .\FreeCADMain.mak CFG="FreeCADMain - Win32 Debug" CLEAN
nmake /F .\FreeCADMain.mak CFG="FreeCADMain - Win32 Debug"
nmake /F .\FreeCADMainCmd.mak CFG="FreeCADMainCmd - Win32 Debug" CLEAN
nmake /F .\FreeCADMainCmd.mak CFG="FreeCADMainCmd - Win32 Debug"
nmake /F .\FreeCADMainPy.mak CFG="FreeCADMainPy - Win32 Debug" CLEAN
nmake /F .\FreeCADMainPy.mak CFG="FreeCADMainPy - Win32 Debug"
cd ..
