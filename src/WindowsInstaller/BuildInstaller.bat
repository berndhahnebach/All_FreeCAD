
if NOT DEFINED WIXDIR set WIXDIR=C:\Program Files (x86)\Windows Installer XML v3

"C:\Program Files\TortoiseSVN\bin\SubWCRev.exe" ..\.. Version.wxi.in Version.wxi
"C:\Program Files\TortoiseSVN\bin\SubWCRev.exe" ..\.. CopyRelease.bat.in CopyRelease.bat
"C:\Program Files\TortoiseSVN\bin\SubWCRev.exe" ..\.. ..\Build\Version.h.in ..\Build\Version.h

SET /P M=Reebuild and press enter 

"%WIXDIR%\bin\candle.exe" -out FreeCADBase.wxobj    FreeCADBase.wxs
"%WIXDIR%\bin\candle.exe" -out LibPack.wxobj        LibPack.wxs
"%WIXDIR%\bin\candle.exe" -out FreeCADDoc.wxobj     FreeCADDoc.wxs
"%WIXDIR%\bin\candle.exe" -out FreeCADModules.wxobj FreeCADModules.wxs
"%WIXDIR%\bin\candle.exe" -out FreeCAD.wxobj        FreeCAD.wxs

"%WIXDIR%\bin\light.exe"  -ext WixUIExtension -cultures:en-us -out FreeCAD.msi *.wxobj

call CopyRelease.bat