
if NOT DEFINED WIXDIR set WIXDIR=C:\Program Files\Windows Installer XML v3

"C:\Program Files\TortoiseSVN\bin\SubWCRev.exe" .. Version.wxi.in Version.wxi

"%WIXDIR%\bin\candle.exe" -out FreeCADBase.wxobj    FreeCADBase.wxs
"%WIXDIR%\bin\candle.exe" -out LibPack.wxobj        LibPack.wxs
"%WIXDIR%\bin\candle.exe" -out FreeCADDoc.wxobj     FreeCADDoc.wxs
"%WIXDIR%\bin\candle.exe" -out FreeCADModules.wxobj FreeCADModules.wxs
"%WIXDIR%\bin\candle.exe" -out FreeCAD.wxobj        FreeCAD.wxs

"%WIXDIR%\bin\light.exe"  -ext WixUIExtension -cultures:en-us -out FreeCAD.msi *.wxobj

